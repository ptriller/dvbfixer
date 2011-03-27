
#include "stream.h"
StreamReader::~StreamReader() {
}

FileStreamReader::FileStreamReader(const std::string &name) {
  stream = new std::fstream(name.c_str(), std::ios_base::in);
}

FileStreamReader::~FileStreamReader() {
  if(stream != NULL) {
    stream->close();
  }
}

unsigned int FileStreamReader::read(void *buf, unsigned int size) {
  if(stream == NULL) return 0;
  return stream->read((char *)buf,size).gcount();
}


void FileStreamReader::close() {
  if(stream != NULL) {
    stream->close();
    delete stream;
    stream = NULL;
  }
}

