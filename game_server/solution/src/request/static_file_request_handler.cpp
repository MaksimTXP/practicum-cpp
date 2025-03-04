#include "static_file_request_handler.h"


namespace http_handler {


std::string StaticFileHandler::GetMimeType(const std::string& extension) {

  auto it = config::MIME_TYPE.find(extension);
  if (it != config::MIME_TYPE.end()) {
    return it->second;
  }
  return std::string(config::ContentType::APPLICATION_OCTET_STREAM);
}

std::string StaticFileHandler::UrlDecode(std::string_view str) {
  std::string result;
  result.reserve(str.size());

  for (size_t i = 0; i < str.size(); ++i) {
    if (str[i] != '%') {
      result += str[i];
    }
    else if (i + 2 < str.size()) {
      char hex[] = { str[i + 1], str[i + 2], '\0' };
      char* end;
      uint64_t symbol = std::strtoul(hex, &end, 16);

      if (*end == '\0') {
        result += static_cast<char>(symbol);
        i += 2;
      }
      else {
        result += str[i];
      }
    }
  }
  return result;
}

bool StaticFileHandler::IsSubPathOfRoot(fs::path path, fs::path base) {

  path = fs::weakly_canonical(path);
  base = fs::weakly_canonical(base);

  for (auto b = base.begin(), p = path.begin(); b != base.end(); ++b, ++p) {
    if (p == path.end() || *p != *b) {
      return false;
    }
  }
  return true;
}


}