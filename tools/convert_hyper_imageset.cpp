// This program converts a set of images to a lmdb/leveldb by storing them
// as Datum proto buffers.
// Usage:
//   convert_hyper_imageset [FLAGS] ROOTFOLDER/ LISTFILE DB_NAME
//
// where ROOTFOLDER is the root folder that holds all the images, and LISTFILE
// should be a list of files as well as their labels, in the format as
//   subfolder1/file1.JPEG 7
//   ....

#include <algorithm>
#include <fstream>  // NOLINT(readability/streams)
#include <string>
#include <utility>
#include <vector>
#include <sstream>
#include <iostream>

#include "boost/scoped_ptr.hpp"
#include "gflags/gflags.h"
#include "glog/logging.h"

#include "caffe/proto/caffe.pb.h"
#include "caffe/util/db.hpp"
#include "caffe/util/io.hpp"
#include "caffe/util/rng.hpp"

using namespace caffe;  // NOLINT(build/namespaces)
using std::pair;
using boost::scoped_ptr;

DEFINE_bool(gray, false,
    "When this option is on, treat images as grayscale ones");
DEFINE_bool(shuffle, false,
    "Randomly shuffle the order of images and their labels");
DEFINE_string(backend, "lmdb",
        "The backend {lmdb, leveldb} for storing the result");
DEFINE_int32(resize_width, 0, "Width images are resized to");
DEFINE_int32(resize_height, 0, "Height images are resized to");
DEFINE_bool(check_size, false,
    "When this option is on, check that all the datum have the same size");
DEFINE_bool(encoded, false,
    "When this option is on, the encoded image will be save in datum");
DEFINE_string(encode_type, "",
    "Optional: What type should we encode the image as ('png','jpg',...).");

int main(int argc, char** argv) {

  ::google::InitGoogleLogging(argv[0]);   //启动log

#ifndef GFLAGS_GFLAGS_H_
  namespace gflags = google;
#endif

  gflags::SetUsageMessage("Convert a set of images to the leveldb/lmdb\n"
        "format used as input for Caffe.\n"
        "Usage:\n"
        "    convert_hyper_imageset [FLAGS] ROOTFOLDER/ LISTFILE DB_NAME\n"
        "The ImageNet dataset for the training demo is at\n"
        "    http://www.image-net.org/download-images\n");
  gflags::ParseCommandLineFlags(&argc, &argv, true);

  if (argc < 4) {
    gflags::ShowUsageWithFlagsRestrict(argv[0], "tools/convert_hyper_imageset");
    return 1;
  }

  const bool is_color = !FLAGS_gray;
  const bool check_size = FLAGS_check_size;
  const bool encoded = FLAGS_encoded;
  const string encode_type = FLAGS_encode_type;

 //const int hyper_num = 2;       // 每张超图由多少个单图像构成
  // const int img_chan_num = 3; // 每张单图像有多少通道
 int num_img = 2;

  std::ifstream infile(argv[2]);
  std::vector<std::pair<std::string, int> > lines;
  std::string filename;
  int label;
  while (infile >> filename >> label) {  // 自动按照空格分列
    lines.push_back(std::make_pair(filename, label));
  }
  if (FLAGS_shuffle) {
    // randomly shuffle data
    LOG(INFO) << "Shuffling data";
    shuffle(lines.begin(), lines.end());
  }
  LOG(INFO) << "A total of " << lines.size() << " images.";

  if (encode_type.size() && !encoded)
    LOG(INFO) << "encode_type specified, assuming encoded=true.";

  int resize_height = std::max<int>(0, FLAGS_resize_height);
  int resize_width = std::max<int>(0, FLAGS_resize_width);



  // Create new DB
  scoped_ptr<db::DB> db(db::GetDB(FLAGS_backend));
  db->Open(argv[3], db::NEW);
  scoped_ptr<db::Transaction> txn(db->NewTransaction());

  // Storing to db
  std::string root_folder(argv[1]);
  Datum datum;
  int count = 0;
  const int kMaxKeyLength = 256;
  char key_cstr[kMaxKeyLength];
  int data_size = 0;
  bool data_size_initialized = false;



  for (int line_id = 0; line_id < lines.size(); ++line_id) {
    bool status;
    std::string enc = encode_type;
    if (encoded && !enc.size()) {
      // Guess the encoding type from the file name
      string fn = lines[line_id].first;
      size_t p = fn.rfind('.');
      if ( p == fn.npos )
        LOG(WARNING) << "Failed to guess the encoding of '" << fn << "'";
      enc = fn.substr(p);
      std::transform(enc.begin(), enc.end(), enc.begin(), ::tolower);
    }

    ///////////////////
    /*
    std::stringstream stream;
    std::string s1;
    for(int i=0; i<num_img; i++) {
    	stream << i;   // int to string
    	s1 = stream.str();
    	std::string hyper_chan = "hyper_chan_" + s1 + "/";
    	stream.clear();
    	stream.str("");
    	filename_vec.push_back(root_folder + hyper_chan + lines[line_id].first);
    }
    */

    std::vector<std::string> filename_vec;
    { // rgb
    	string filename_temp = lines[line_id].first;

    	string num = filename_temp.substr(filename_temp.length() - 10, 6);
    	stringstream ss;
      	ss<<num;
      	int i;
      	ss>>i;  // string to int
      	ss.clear();
      	ss.str("");
     	 i += 2;
      	char buffer[200];
      	sprintf(buffer, "%06d", i);//转换123为"00123"
      	puts(buffer); // 从某一址开始，依次输出存储单元中的字符。

      	string num_result = buffer;  // char数组转化为string 直接赋值即可
      	string path = filename_temp.substr(0, filename_temp.length() - 10);
        string format = filename_temp.substr(filename_temp.length() - 4, 4);
        string result = path + num_result + format;
        // std::cout << result << std::endl;
    	filename_vec.push_back(root_folder + "hyper_chan_0/" + result); // rgb
    }
    filename_vec.push_back(root_folder + "hyper_chan_1/" + lines[line_id].first); // flow
    //std::cout <<  filename_vec[0] << std::endl;


    status = ReadHyperImageToDatum(filename_vec, num_img, lines[line_id].second, resize_height, resize_width, is_color, enc, &datum);
    ///////////////////

    if (status == false)  continue;
    if (check_size) {
      if (!data_size_initialized) {
        data_size = datum.channels() * datum.height() * datum.width();
        data_size_initialized = true;
      } else {
        const std::string& data = datum.data();
        CHECK_EQ(data.size(), data_size) << "Incorrect data field size " << data.size();
      }
    }
    // sequential
    int length = snprintf(key_cstr, kMaxKeyLength, "%08d_%s", line_id, lines[line_id].first.c_str());

    // Put in db
    string out;
    CHECK(datum.SerializeToString(&out));
    txn->Put(string(key_cstr, length), out);

    if (++count % 1000 == 0) {
      // Commit db
      txn->Commit();
      txn.reset(db->NewTransaction());
      LOG(ERROR) << "Processed " << count << " files.";
    }
  }
  // write the last batch
  if (count % 1000 != 0) {
    txn->Commit();
    LOG(ERROR) << "Processed " << count << " files.";
  }
  return 0;
}
