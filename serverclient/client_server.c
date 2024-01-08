#include "client.h"
#include "server.h"
#include "client_server.h"
#include <stdio.h>
#include <string.h>
#include <openssl/evp.h>
#include <openssl/err.h>

void handleErrors(void)
{
    ERR_print_errors_fp(stderr);
    abort();
}

void encrypt_decrypt(EVP_CIPHER_CTX *ctx, const unsigned char *in, int inl, unsigned char *out, int *outl)
{
    if (!EVP_CipherUpdate(ctx, out, outl, in, inl)) {
        fprintf(stderr, "Erreur lors du chiffrement/dÃ©chiffrement\n");
        handleErrors();
    }
}

int sendmessage(char msg[1024], int port)
{
    // Encryption
    EVP_CIPHER_CTX *ctx;
    unsigned char key[EVP_MAX_KEY_LENGTH] = "abcdefghijklmnopqrstuvwxyz ,;.ABCDEFGHIJKLMNOPQRSTUVWXYZABCD\0";
    unsigned char iv[EVP_MAX_IV_LENGTH] = "0123456789012345";
    unsigned char encrypted_msg[1024 + EVP_MAX_BLOCK_LENGTH] = "\0";
    int encrypted_msg_len = 0;

    OpenSSL_add_all_ciphers();
    ERR_load_crypto_strings();

    if (!(ctx = EVP_CIPHER_CTX_new()))
        handleErrors();

    if (!EVP_CipherInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv, 1))
        handleErrors();

    encrypt_decrypt(ctx, (const unsigned char *)msg, strlen((char *)msg) + 1, encrypted_msg, &encrypted_msg_len);
    // Sending the encrypted message
    sndmsg((char *)msg, port);

    EVP_CIPHER_CTX_free(ctx);

    return strlen(msg);
}

int start_server(int port)
{
    startserver(port);
    return port;
}

int stop_server()
{
    stopserver();
    return 0;
}

int getmessage(char msg_read[1024])
{
    // Receive the encrypted message
    getmsg(msg_read);

    // Decryption
    EVP_CIPHER_CTX *ctx;
    unsigned char key[EVP_MAX_KEY_LENGTH] = "abcdefghijklmnopqrstuvwxyz ,;.ABCDEFGHIJKLMNOPQRSTUVWXYZABCD\0";
    unsigned char iv[EVP_MAX_IV_LENGTH] = "0123456789012345";
    unsigned char decrypted_msg[1024 + EVP_MAX_BLOCK_LENGTH] = "\0";
    int decrypted_msg_len = 0;

    OpenSSL_add_all_ciphers();
    ERR_load_crypto_strings();

    if (!(ctx = EVP_CIPHER_CTX_new()))
        handleErrors();

    if (!EVP_CipherInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv, 0))
        handleErrors();

    encrypt_decrypt(ctx, (const unsigned char *)msg_read, strlen((char *)msg_read) + 1, decrypted_msg, &decrypted_msg_len);

    // Copy the decrypted message to msg_read
    strncpy(msg_read, (char *)decrypted_msg, decrypted_msg_len);
    msg_read[decrypted_msg_len] = '\0';

    EVP_CIPHER_CTX_free(ctx);

    return strlen(msg_read);
}