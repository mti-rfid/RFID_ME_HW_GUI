#ifndef QRFECRC_H
#define QRFECRC_H

#include <QtGlobal>
#include <QByteArray>
#include <QString>

typedef struct
{
        unsigned long int state[4];   	      /* state (ABCD) */
        unsigned long int count[2]; 	      /* number of bits, modulo 2^64 (lsb first) */
        unsigned char buffer[64];	      /* input buffer */
} MD5_CTX;


class QrfeCRC
{
public:
    //static quint64 calcCRC ( QByteArray data, QString mode );
    static quint8       calcXORCS ( QByteArray data );
    static QByteArray   calcMD5   ( QByteArray data );

private:

    static void MD5Init (MD5_CTX*);
    static void MD5Update (MD5_CTX*, uchar*, uint);
    static void MD5Final (uchar [16], MD5_CTX*);

    static void MD5Transform (ulong state[4], uchar block[64]);
    static void Encode (uchar*, ulong*, uint);
    static void Decode (ulong*, uchar*, uint);
    static void MD5_memcpy (uchar*, uchar*, uint);
    static void MD5_memset (uchar*, int, uint);

};

#endif // QRFECRC_H
