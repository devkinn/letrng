#include <iostream>

#include <cryptopp/rsa.h>
#include <cryptopp/sha.h>
#include <cryptopp/filters.h>
#include <cryptopp/files.h>
#include <cryptopp/hex.h>
#include <cryptopp/pssr.h>

#include "letrng_crypto.h"

using namespace CryptoPP;

void GenerateRSAKeyPair(RSA::PrivateKey &privateKey, RSA::PublicKey &publicKey)
{
	LetrngCryptoPP rng;

	InvertibleRSAFunction params;
	params.GenerateRandomWithKeySize(rng, 3072);

	privateKey = RSA::PrivateKey(params);
	publicKey = RSA::PublicKey(params);
}

std::string SignMessage(const std::string &message, const RSA::PrivateKey &privateKey)
{
	LetrngCryptoPP rng;

	RSASS<PSSR, SHA256>::Signer signer(privateKey);

	std::string signature;
	StringSource ss(message, true,
					new SignerFilter(rng, signer,
									 new StringSink(signature)));

	return signature;
}

bool VerifySignature(const std::string &message, const std::string &signature, const RSA::PublicKey &publicKey)
{
	RSASS<PSSR, SHA256>::Verifier verifier(publicKey);

	try
	{
		StringSource ss(message + signature, true,
						new SignatureVerificationFilter(
							verifier, NULL,
							SignatureVerificationFilter::THROW_EXCEPTION) // SignatureVerificationFilter
		);																  // StringSource
		return true;
	}
	catch (const Exception &e)
	{
		std::cerr << "Signature verification failed: " << e.what() << std::endl;
		return false;
	}
}

int main()
{
	RSA::PrivateKey privateKey;
	RSA::PublicKey publicKey;

	GenerateRSAKeyPair(privateKey, publicKey);

	std::string message = "A very important message.";

	std::string signature = SignMessage(message, privateKey);

	std::cout << "Message sent: " << message << std::endl;

	std::cout << "Signature (hex): ";
	StringSource(signature, true, new HexEncoder(new FileSink(std::cout)));
	std::cout << std::endl;

	std::cout << "Message received: " << message << std::endl;

	bool isValid = VerifySignature(message, signature, publicKey);

	std::cout << "Signature verification: " << (isValid ? "successful" : "failed") << std::endl;

	return 0;
}