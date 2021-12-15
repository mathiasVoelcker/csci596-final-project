#pragma once
#include "httplib.h"
constexpr auto URL = "earthquake.usgs.gov";
constexpr auto CA_CERT_FILE = "./ca-bundle.crt";
namespace Util
{
	httplib::Result query(const httplib::Params& params);
};

