#include <glog/logging.h>

int main(int argc, char **argv) {
  FLAGS_logtostderr = true; // Output logs to standard error
  FLAGS_minloglevel = 0;    // Log all severity levels (including ERROR)
  google::InitGoogleLogging(argv[0]);
  LOG(INFO) << "HI";
  LOG(ERROR) << "HI2";
  return 0;
}