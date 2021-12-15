#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "Util.h"

namespace Util
{
	httplib::Result query(const httplib::Params& params) {
		httplib::SSLClient cli(URL);
		cli.set_ca_cert_path(CA_CERT_FILE);
		cli.enable_server_certificate_verification(true);

		std::string path_with_query = httplib::append_query_params("/fdsnws/event/1/query", params);
		return cli.Get(path_with_query.c_str());
	}

	void Util::from_json(const json& j, properties& p)
	{
		std::stringstream ss;

		ss << j["properties"]["mag"];
		ss >> p.mag;
		ss.clear();

		ss << j["geometry"]["coordinates"][0];
		ss >> p.lat;
		ss.clear();

		ss << j["geometry"]["coordinates"][1];
		ss >> p.longitude;
		ss.clear();

		ss << j["geometry"]["coordinates"][2];
		ss >> p.depth;
	}
};
