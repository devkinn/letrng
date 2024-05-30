#include <iostream>
#include <fstream>
#include <string>
#include <cryptopp/sha.h>
#include <cryptopp/filters.h>
#include <cryptopp/files.h>
#include <cryptopp/hex.h>
#include <cryptopp/osrng.h>
#include <cryptopp/rsa.h>
#include <cryptopp/pssr.h>
#include <cryptopp/base64.h>

#include "letrng_crypto.h"

using namespace CryptoPP;

void GenerateSHA256(const std::string &filename, std::string &digest)
{
	SHA256 hash;
	FileSource file(filename.c_str(), true,
					new HashFilter(hash,
								   new HexEncoder(
									   new StringSink(digest))));
}

void GenerateKeyPair(RSA::PrivateKey &privateKey, RSA::PublicKey &publicKey)
{
	LetrngCryptoPP rng;
	privateKey.GenerateRandomWithKeySize(rng, 3072);
	publicKey = RSA::PublicKey(privateKey);
}

void Encode(const std::string &filename, const BufferedTransformation &bt)
{
	FileSink file(filename.c_str());

	bt.CopyTo(file);
	file.MessageEnd();
}

void EncodePrivateKey(const std::string &filename, const RSA::PrivateKey &key)
{
	ByteQueue queue;
	key.DEREncodePrivateKey(queue);

	Encode(filename, queue);
}

void EncodePublicKey(const std::string &filename, const RSA::PublicKey &key)
{
	ByteQueue queue;
	key.DEREncodePublicKey(queue);

	Encode(filename, queue);
}

void Decode(const std::string &filename, BufferedTransformation &bt)
{
	FileSource file(filename.c_str(), true /*pumpAll*/);

	file.TransferTo(bt);
	bt.MessageEnd();
}

void DecodePrivateKey(const std::string &filename, RSA::PrivateKey &key)
{
	ByteQueue queue;

	Decode(filename, queue);
	key.BERDecodePrivateKey(queue, false /*paramsPresent*/, queue.MaxRetrievable());
}

void DecodePublicKey(const std::string &filename, RSA::PublicKey &key)
{
	ByteQueue queue;

	Decode(filename, queue);
	key.BERDecodePublicKey(queue, false /*paramsPresent*/, queue.MaxRetrievable());
}

void SignFile(const std::string &filename, const RSA::PrivateKey &privateKey, std::string &signature)
{
	LetrngCryptoPP rng;

	std::string digest;
	GenerateSHA256(filename, digest);

	RSASS<PSS, SHA256>::Signer signer(privateKey);
	StringSource ss(digest, true,
					new SignerFilter(rng, signer,
									 new StringSink(signature)));
}

bool VerifyFile(const std::string &filename, const RSA::PublicKey &publicKey, const std::string &signature)
{
	LetrngCryptoPP rng;

	std::string digest;
	GenerateSHA256(filename, digest);

	RSASS<PSS, SHA256>::Verifier verifier(publicKey);
	bool result = false;
	StringSource ss(signature + digest, true,
					new SignatureVerificationFilter(verifier,
													new ArraySink((byte *)&result, sizeof(result))));

	return result;
}

void SaveSignature(const std::string &filename, const std::string &signature)
{
	StringSource(signature, true, new FileSink(filename.c_str()));
}

void LoadSignature(const std::string &filename, std::string &signature)
{
	FileSource(filename.c_str(), true, new StringSink(signature));
}

int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		std::cerr << "Usage: " << argv[0] << " -g | <file> -s | <file> -sk <private_key_file> | <file> -v <signature_file> <public_key_file>" << std::endl;
		return 1;
	}

	std::string mode;
	std::string file = ",";

	for (int i = 1; i < argc; ++i)
	{
		std::string arg = argv[i];
		if (arg == "-g" || arg == "-s" || arg == "-sk" || arg == "-v")
		{
			mode = arg;
		}
		else if (file == ",")
		{
			file = arg;
		}
	}

	try
	{
		if (mode == "-g" && argc == 2)
		{
			RSA::PrivateKey privateKey;
			RSA::PublicKey publicKey;
			GenerateKeyPair(privateKey, publicKey);

			EncodePrivateKey("rsa-private.key", privateKey);
			EncodePublicKey("rsa-public.key", publicKey);
			std::cout << "Key pair generated. Private key saved to rsa-private.key, Public key saved to rsa-public.key" << std::endl;
		}
		else if (mode == "-s" && argc == 3)
		{
			RSA::PrivateKey privateKey;
			RSA::PublicKey publicKey;
			GenerateKeyPair(privateKey, publicKey);

			EncodePrivateKey("rsa-private.key", privateKey);
			EncodePublicKey("rsa-public.key", publicKey);

			std::string signature;
			SignFile(file, privateKey, signature);

			SaveSignature(file + ".sig", signature);

			std::cout << "Key pair generated. Private key saved to rsa-private.key, Public key saved to rsa-public.key" << std::endl;
			std::cout << "File signed. Signature saved to " << file + ".sig" << std::endl;
		}
		else if (mode == "-sk" && argc == 4)
		{
			std::string privateKeyFile = argv[3];

			RSA::PrivateKey privateKey;
			DecodePrivateKey(privateKeyFile, privateKey);

			std::string signature;
			SignFile(file, privateKey, signature);

			SaveSignature(file + ".sig", signature);
			std::cout << "File signed. Signature saved to " << file + ".sig" << std::endl;
		}
		else if (mode == "-v" && argc == 5)
		{
			std::string signatureFile = argv[3];
			std::string publicKeyFile = argv[4];

			RSA::PublicKey publicKey;
			DecodePublicKey(publicKeyFile, publicKey);

			std::string signature;
			LoadSignature(signatureFile, signature);

			std::cout << "Signature loaded from " << signatureFile << std::endl;
			std::cout << "Public key loaded from " << publicKeyFile << std::endl;

			if (VerifyFile(file, publicKey, signature))
			{
				std::cout << "Signature is valid." << std::endl;
			}
			else
			{
				std::cout << "Signature is invalid." << std::endl;
			}
		}
		else
		{
			std::cerr << "Invalid arguments." << std::endl;
			return 1;
		}
	}
	catch (const Exception &e)
	{
		std::cerr << "Crypto++ error: " << e.what() << std::endl;
		return 1;
	}
	catch (const std::exception &e)
	{
		std::cerr << "Standard error: " << e.what() << std::endl;
		return 1;
	}
	catch (...)
	{
		std::cerr << "Unknown error." << std::endl;
		return 1;
	}

	return 0;
}
