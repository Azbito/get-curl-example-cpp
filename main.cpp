#include <iostream>
#include <sstream>
#include <string>
#include <curl/curl.h>
#include <json/json.h>

// callback is used to get data from http req
static size_t writeCallback(void* contents, size_t size, size_t nmemb, std::string* buffer) {
	buffer->append((char*)contents, size * nmemb);
	return size * nmemb;
}

int main() {
	CURL* curl;
	CURLcode res;
	std::string readBuffer;

	curl_global_init(CURL_GLOBAL_DEFAULT);
	curl = curl_easy_init();

	if (curl) {
		std::string zipCode;

		std::cout << "Buscar CEP: ";
		std::cin >> zipCode;

		if (zipCode.empty() || zipCode.length() >= 10) {
			std::cout << u8"\nPor favor, digite um CEP válido: ";
			std::cin >> zipCode;
		}


		if (zipCode.length() == 9) {
			zipCode.erase(5, 1);
		}

		std::stringstream API_URL;
		API_URL << "https://viacep.com.br/ws/" << zipCode << "/json/";

		// Configures to req
		curl_easy_setopt(curl, CURLOPT_URL, API_URL.str().c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

		// Execute the req
		res = curl_easy_perform(curl);

		if (res != CURLE_OK) {
			std::cerr << "Erro na requisição: " << curl_easy_strerror(res) << std::endl;
		}
		else
		{
			Json::CharReaderBuilder builder;
			Json::CharReader* reader = builder.newCharReader();
			Json::Value root;
			std::string errors;

			if (!reader->parse(readBuffer.c_str(), readBuffer.c_str() + readBuffer.size(), &root, &errors)) {
				std::cerr << "Erro ao analisar JSON: " << errors << std::endl;
			}
			else {
				std::cout << "CEP: " << root["cep"].asString() << std::endl;
				std::cout << "Logradouro: " << root["logradouro"].asString() << std::endl;
				std::cout << "Complemento: " << root["complemento"].asString() << std::endl;
				std::cout << "Unidade: " << root["unidade"].asString() << std::endl;
				std::cout << "Bairro: " << root["bairro"].asString() << std::endl;
				std::cout << "Localidade: " << root["localidade"].asString() << std::endl;
				std::cout << "Estado: " << root["uf"].asString() << std::endl;
				std::cout << "IBGE: " << root["ibge"].asString() << std::endl;
				std::cout << "GIA: " << root["gia"].asString() << std::endl;
				std::cout << "DDD: " << root["ddd"].asString() << std::endl;
				std::cout << "SIAFI: " << root["Siafi"].asString() << std::endl;
			}

			delete reader;
		}

		curl_easy_cleanup(curl);
	}

	curl_global_cleanup();

	return 0;
}
