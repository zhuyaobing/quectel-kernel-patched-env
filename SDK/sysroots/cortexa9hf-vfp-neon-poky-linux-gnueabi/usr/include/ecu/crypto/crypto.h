/*
 * crypto.h
 *
 *  Created on: Dec 22, 2016
 *      Author: hamk
 */

#ifndef SRC_CRYPTO_H_
#define SRC_CRYPTO_H_

#include <vector>
#include <string>
#include <mutex>

#include <openssl/evp.h>

namespace ecu {
namespace crypto {

/**
 * @brief Class handling symmetric (AES) encryption/decryption of strings
 *
 * @details Wraps calls to either sm (imx secure module)
 *          hardware encryption or openssl aes-256 encryption.
 * @note The device is opened in the constructor and closed in the destructor
 *
 */
class Crypto
{
   public:
      enum Method{
         CAAM,
         OPENSSL,
      };

      Crypto(enum Method be = CAAM);
      virtual ~Crypto();

      /**
      * @brief Set salt base for use in openssl encrypt and decrypt
      * @details The input string is hashed and then used to generate key and IV
      * @throws runtime_error
      */
      void set_salt_base(const std::string &base);

      /**
      * @brief Encrypt in data string and returns encrypted string
      *        base64 encoded
      * @details The input string is chuncked into pieces of max 480 bytes
      *          which is the size the SM-module can handle.
      *          The output is base64 encoded with special delimiter character
      *          between the "chunks".
      * @throws runtime_error
      */
      std::string encrypt(const std::string& in);

      /**
      * @brief Decrypt in data string and return encrypted
      * data. Input string is base64 encoded chuncks separated
      *       with a delimiter character (non-base64)
      * @throws runtime_error
      */
      std::string decrypt(const std::string& in);

      /**
      * @brief Calculate digest of input string
      * @details Digest str with md_str digest type, supports sha{1,224,256,384,512}.
      *          The result is put in array md_val of length EVP_MAX_MD_SIZE
      * @return 0 on failure otherwise length in bytes of the digest in md_val
      * @throws runtime_error
      */
      int digest(const std::string &md_str, const std::string &str, unsigned char *md_val);

   private:
     // Throws runtime_error if key and IV not set for Method::OPENSSL
     bool has_key_and_iv();
     std::string encrypt_part_caam(const std::string& in);
     std::string decrypt_part_caam(const std::string& in);
     std::string encrypt_part_openssl(const std::string& in);
     std::string decrypt_part_openssl(const std::string& in);
     void open_dev();
     void close_dev();
     int m_fd;
     enum Method m_method;
     std::array<unsigned char, EVP_MAX_KEY_LENGTH> m_key;
     std::array<unsigned char, EVP_MAX_IV_LENGTH> m_iv;

     // Global mutex lock of device
     static std::mutex g_dev_mutex;
};



/**
 * @brief stores a data blob as base64 encoded data to a file.
 * @note Limited to BUFFER_SIZE
 */
void store_file_base64(const std::string& file, const std::string& data);

/**
 * @brief reads and decodes a base64 data from a file
 * @note Limited to BUFFER_SIZE
 */
std::string read_file_base64(const std::string& file);

/**
 * @brief base64 encode a data blobs
 */
std::string base64_encode(const std::string& data);

/**
 * @brief decode a base64 data blob
 */
std::string base64_decode(const std::string& data);

/**
 * @brief splits a string into substrings on delimiter.
 * @note Limited to BUFFER_SIZE
 */
std::vector<std::string> split(const std::string& s, const std::string& delimiters);


} /* namespace crypto */
} /* namespace ecu */
#endif /* SRC_CRYPTO_H_  */
