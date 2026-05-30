#include "functionsforserver.h"

#include <QCryptographicHash>
#include <QImage>
#include <QStringList>
#include <cmath>

#include <openssl/evp.h>
#include <openssl/err.h>

// f(x) = x^3 - x - 2  (root ≈ 1.5213797)
static double f(double x) { return x * x * x - x - 2.0; }

// ─── Protocol dispatcher ────────────────────────────────────────────────────

QString parsing(const QString& command)
{
    if (command.isEmpty()) return "error";

    const QStringList parts = command.split(':');
    const QString cmd = parts[0].toUpper().trimmed();

    if (cmd == "SHA384" && parts.size() >= 2)
        return sha384Hash(parts.mid(1).join(':'));

    if (cmd == "AES_ENCRYPT" && parts.size() >= 3)
        return aesEncrypt(parts[1], parts.mid(2).join(':'));

    if (cmd == "AES_DECRYPT" && parts.size() >= 3)
        return aesDecrypt(parts[1], parts[2]);

    if (cmd == "CHORD" && parts.size() >= 4) {
        bool ok1, ok2, ok3;
        double a   = parts[1].toDouble(&ok1);
        double b   = parts[2].toDouble(&ok2);
        double eps = parts[3].toDouble(&ok3);
        if (!ok1 || !ok2 || !ok3) return "error";
        return chordMethod(a, b, eps);
    }

    if (cmd == "STEG_EMBED" && parts.size() >= 4)
        return steganographyEmbed(parts[1], parts[2], parts.mid(3).join(':'));

    if (cmd == "STEG_EXTRACT" && parts.size() >= 2)
        return steganographyExtract(parts[1]);

    return "error";
}

// ─── AES-128-CBC (OpenSSL EVP) ───────────────────────────────────────────────

static QByteArray makeKey(const QString& key)
{
    QByteArray k(16, '\0');
    const QByteArray src = key.toUtf8().left(16);
    memcpy(k.data(), src.constData(), src.size());
    return k;
}

static const QByteArray kIV(16, '\0');  // fixed IV — suitable for a demo

QString aesEncrypt(const QString& key, const QString& plaintext)
{
    QByteArray keyBytes  = makeKey(key);
    QByteArray plainBytes = plaintext.toUtf8();

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) return "error";

    if (EVP_EncryptInit_ex(ctx, EVP_aes_128_cbc(), nullptr,
                           reinterpret_cast<const unsigned char*>(keyBytes.constData()),
                           reinterpret_cast<const unsigned char*>(kIV.constData())) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return "error";
    }

    QByteArray out(plainBytes.size() + 16, '\0');
    int len = 0, finalLen = 0;

    EVP_EncryptUpdate(ctx,
                      reinterpret_cast<unsigned char*>(out.data()), &len,
                      reinterpret_cast<const unsigned char*>(plainBytes.constData()),
                      plainBytes.size());

    EVP_EncryptFinal_ex(ctx,
                        reinterpret_cast<unsigned char*>(out.data()) + len, &finalLen);

    EVP_CIPHER_CTX_free(ctx);
    out.resize(len + finalLen);
    return QString::fromLatin1(out.toHex());
}

QString aesDecrypt(const QString& key, const QString& ciphertextHex)
{
    QByteArray keyBytes   = makeKey(key);
    QByteArray cipherBytes = QByteArray::fromHex(ciphertextHex.toLatin1());

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) return "error";

    if (EVP_DecryptInit_ex(ctx, EVP_aes_128_cbc(), nullptr,
                           reinterpret_cast<const unsigned char*>(keyBytes.constData()),
                           reinterpret_cast<const unsigned char*>(kIV.constData())) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return "error";
    }

    QByteArray out(cipherBytes.size(), '\0');
    int len = 0, finalLen = 0;

    EVP_DecryptUpdate(ctx,
                      reinterpret_cast<unsigned char*>(out.data()), &len,
                      reinterpret_cast<const unsigned char*>(cipherBytes.constData()),
                      cipherBytes.size());

    if (EVP_DecryptFinal_ex(ctx,
                            reinterpret_cast<unsigned char*>(out.data()) + len,
                            &finalLen) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return "error: decryption failed (bad key or corrupt data)";
    }

    EVP_CIPHER_CTX_free(ctx);
    out.resize(len + finalLen);
    return QString::fromUtf8(out);
}

// ─── SHA-384 ─────────────────────────────────────────────────────────────────

QString sha384Hash(const QString& data)
{
    return QString::fromLatin1(
        QCryptographicHash::hash(data.toUtf8(), QCryptographicHash::Sha384).toHex());
}

// ─── Method of chords (Regula Falsi) for f(x) = x³ - x - 2 ─────────────────

QString chordMethod(double a, double b, double eps)
{
    if (f(a) * f(b) >= 0)
        return "error: f(a) and f(b) must have opposite signs";

    double x    = a;
    double xPrev = b;
    const int maxIter = 100000;

    for (int i = 0; i < maxIter; ++i) {
        double fa = f(a), fb = f(b);
        xPrev = x;
        x = a - fa * (b - a) / (fb - fa);

        if (f(a) * f(x) <= 0)
            b = x;
        else
            a = x;

        if (std::fabs(x - xPrev) < eps) break;
    }

    return QString::number(x, 'f', 10);
}

// ─── Steganography (LSB in RGB channels) ─────────────────────────────────────

// Layout: [4 bytes len (big-endian)] [message bytes]
// Each bit stored in the LSB of R, G, B channels sequentially.

QString steganographyEmbed(const QString& srcPath, const QString& dstPath, const QString& message)
{
    QImage img(srcPath);
    if (img.isNull())
        return "error: cannot load " + srcPath;

    img = img.convertToFormat(QImage::Format_ARGB32);

    QByteArray msgBytes = message.toUtf8();
    const int msgLen    = msgBytes.size();

    const int bitsNeeded    = (4 + msgLen) * 8;
    const int bitsAvailable = img.width() * img.height() * 3;
    if (bitsNeeded > bitsAvailable)
        return "error: message too long for the given image";

    QByteArray payload;
    payload.reserve(4 + msgLen);
    payload.append(static_cast<char>((msgLen >> 24) & 0xFF));
    payload.append(static_cast<char>((msgLen >> 16) & 0xFF));
    payload.append(static_cast<char>((msgLen >>  8) & 0xFF));
    payload.append(static_cast<char>( msgLen        & 0xFF));
    payload.append(msgBytes);

    const int totalBits = payload.size() * 8;
    int bitIdx = 0;

    auto bit = [&](int idx) -> int {
        return (static_cast<unsigned char>(payload[idx / 8]) >> (7 - idx % 8)) & 1;
    };

    for (int y = 0; y < img.height() && bitIdx < totalBits; ++y) {
        for (int x = 0; x < img.width() && bitIdx < totalBits; ++x) {
            const QRgb px = img.pixel(x, y);
            int r = qRed(px), g = qGreen(px), b = qBlue(px);

            if (bitIdx < totalBits) r = (r & ~1) | bit(bitIdx++);
            if (bitIdx < totalBits) g = (g & ~1) | bit(bitIdx++);
            if (bitIdx < totalBits) b = (b & ~1) | bit(bitIdx++);

            img.setPixel(x, y, qRgba(r, g, b, qAlpha(px)));
        }
    }

    if (!img.save(dstPath))
        return "error: cannot save to " + dstPath;

    return "ok: " + dstPath;
}

QString steganographyExtract(const QString& imagePath)
{
    QImage img(imagePath);
    if (img.isNull())
        return "error: cannot load " + imagePath;

    img = img.convertToFormat(QImage::Format_ARGB32);

    // Collect LSBs of R, G, B for every pixel
    QVector<int> bits;
    bits.reserve(img.width() * img.height() * 3);

    for (int y = 0; y < img.height(); ++y)
        for (int x = 0; x < img.width(); ++x) {
            const QRgb px = img.pixel(x, y);
            bits.append(qRed(px)   & 1);
            bits.append(qGreen(px) & 1);
            bits.append(qBlue(px)  & 1);
        }

    auto bitsToInt = [&](int startBit, int n) -> int {
        int val = 0;
        for (int i = 0; i < n; ++i) {
            if (startBit + i >= bits.size()) return -1;
            val = (val << 1) | bits[startBit + i];
        }
        return val;
    };

    if (bits.size() < 32)
        return "error: image too small";

    const int msgLen = bitsToInt(0, 32);
    if (msgLen < 0 || msgLen > 10'000'000)
        return "error: invalid length header";

    if (bits.size() < 32 + msgLen * 8)
        return "error: image too small for claimed message length";

    QByteArray result;
    result.reserve(msgLen);
    for (int i = 0; i < msgLen; ++i) {
        const int byte = bitsToInt(32 + i * 8, 8);
        if (byte < 0) return "error: extraction failed";
        result.append(static_cast<char>(byte));
    }

    return QString::fromUtf8(result);
}
