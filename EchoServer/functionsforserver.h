#ifndef FUNCTIONSFORSERVER_H
#define FUNCTIONSFORSERVER_H

#include <QString>

// Main protocol dispatcher: returns "error" for unrecognised commands.
// Protocol (colon-separated):
//   SHA384:<data>
//   AES_ENCRYPT:<key>:<plaintext>
//   AES_DECRYPT:<key>:<ciphertext_hex>
//   CHORD:<a>:<b>:<eps>          — solves f(x)=x³-x-2 on [a,b]
//   STEG_EMBED:<src>:<dst>:<msg> — embeds msg into src, saves to dst
//   STEG_EXTRACT:<path>          — extracts embedded message
QString parsing(const QString& command);

QString aesEncrypt(const QString& key, const QString& plaintext);
QString aesDecrypt(const QString& key, const QString& ciphertextHex);
QString sha384Hash(const QString& data);
QString chordMethod(double a, double b, double eps);
QString steganographyEmbed(const QString& srcPath, const QString& dstPath, const QString& message);
QString steganographyExtract(const QString& imagePath);

#endif // FUNCTIONSFORSERVER_H
