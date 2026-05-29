#include <QtTest>
#include "../EchoServer/functionsforserver.h"

class FuncForServer_Test : public QObject
{
    Q_OBJECT

public:
    FuncForServer_Test();
    ~FuncForServer_Test();

private slots:
    void test_parsing_invalid();
    void test_sha384();
    void test_aes_roundtrip();
    void test_chord();
};

FuncForServer_Test::FuncForServer_Test() {}
FuncForServer_Test::~FuncForServer_Test() {}

void FuncForServer_Test::test_parsing_invalid()
{
    QVERIFY2(parsing("ewqeewqewq") == "error",
             "parsing(\"ewqeewqewq\") != \"error\"");
    QVERIFY2(parsing("") == "error",
             "parsing(\"\") != \"error\"");
    QVERIFY2(parsing("UNKNOWN:data") == "error",
             "parsing(\"UNKNOWN:data\") != \"error\"");
}

void FuncForServer_Test::test_sha384()
{
    // SHA-384 of empty string is a known constant
    const QString expected =
        "38b060a751ac96384cd9327eb1b1e36a21fdb71114be07434c0cc7bf63f6e1da"
        "274edebfe76f65fbd51ad2f14898b95b";
    QVERIFY2(sha384Hash("") == expected, "SHA-384(\"\") mismatch");

    // parsing() route
    QVERIFY2(parsing("SHA384:") == expected, "parsing(SHA384:) mismatch");
}

void FuncForServer_Test::test_aes_roundtrip()
{
    const QString key  = "secretkey";
    const QString text = "Hello, World!";

    const QString cipher  = aesEncrypt(key, text);
    QVERIFY2(cipher != "error", "AES encrypt returned error");
    QVERIFY2(!cipher.isEmpty(), "AES encrypt returned empty string");

    const QString plain = aesDecrypt(key, cipher);
    QVERIFY2(plain == text, qPrintable("AES round-trip failed: got " + plain));
}

void FuncForServer_Test::test_chord()
{
    // f(x) = x^3 - x - 2, root on [1, 2] ≈ 1.5213797
    const QString res = chordMethod(1.0, 2.0, 1e-9);
    QVERIFY2(res != "error", "chordMethod returned error");

    bool ok;
    double root = res.toDouble(&ok);
    QVERIFY2(ok, "chordMethod result is not a number");
    QVERIFY2(std::fabs(root - 1.5213797) < 1e-5,
             qPrintable(QString("Root %1 is too far from expected 1.5213797").arg(root)));
}

QTEST_APPLESS_MAIN(FuncForServer_Test)

#include "tst_funcforserver_test.moc"
