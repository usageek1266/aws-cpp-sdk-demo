#include <aws/s3/S3Client.h>
#include <aws/s3/model/PutObjectRequest.h>
#include <aws/s3/model/GetObjectRequest.h>
#include <aws/core/Aws.h>
#include <aws/core/utils/memory/stl/AWSStringStream.h>
#include <aws/core/auth/AWSCredentialsProvider.h>
#include <aws/s3/model/CreateBucketRequest.h>
#include <aws/s3/model/DeleteBucketRequest.h>
#include <aws/s3/model/DeleteObjectRequest.h>
#include <gtest/gtest.h>

#include <aws/core/utils/logging/DefaultLogSystem.h>
#include <aws/core/utils/logging/ConsoleLogSystem.h>
#include <aws/core/utils/logging/AWSLogging.h>


using namespace Aws::S3;
using namespace Aws::S3::Model;

static const char *KEY = "1.txt";
static const char *BUCKET = "test1";

class FooTest : public ::testing::Test {

protected:
  S3Client * pclient;
  Aws::SDKOptions options;

  FooTest() {}
  virtual ~FooTest() {}

  virtual void SetUp() {
    Aws::InitAPI(options);
    Aws::Client::ClientConfiguration clientConfig;
    clientConfig.endpointOverride = "127.0.0.1:8000";
    clientConfig.scheme = Aws::Http::Scheme::HTTP;
    clientConfig.region = Aws::String("us-east-1");
    pclient = new Aws::S3::S3Client(Aws::Auth::AWSCredentials("yly", "yly"),
                                    clientConfig,
                                    Aws::Client::AWSAuthV4Signer::PayloadSigningPolicy::Never,
                                    false);
  }
  virtual void TearDown() {
    Aws::ShutdownAPI(options);
    delete pclient;
  }
};

TEST_F(FooTest, CreateBucket) {
  CreateBucketRequest request;
  request.SetBucket("test2");
  pclient->CreateBucket(request);
}

TEST_F(FooTest, PutObjectRequest) {
//first put an object into s3
  PutObjectRequest request;
  request.WithBucket("test1").WithKey("object1");

  //this can be any arbitrary stream (e.g. fstream, stringstream etc...)
  auto requestStream = Aws::MakeShared<Aws::StringStream>("s3-sample");
  *requestStream << "Hello World!";

  //set the stream that will be put to s3
  request.SetBody(requestStream);

  auto putObjectOutcome = pclient->PutObject(request);

  if (putObjectOutcome.IsSuccess()) {
    std::cout << "Successfully" << std::endl;
  } else {
    std::cout << putObjectOutcome.GetError().GetExceptionName() <<
              " " << putObjectOutcome.GetError().GetMessage() << std::endl;
  }
}

TEST_F(FooTest, GetObjectRequest) {
  GetObjectRequest request;
  request.WithBucket("test1").WithKey("object1");

  auto getObjectOutcome = pclient->GetObject(request);

  if (getObjectOutcome.IsSuccess()) {
    std::cout << "Successfully retrieved object from s3 with value: " << std::endl;
    std::cout << getObjectOutcome.GetResult().GetBody().rdbuf() << std::endl << std::endl;;
  } else {
    std::cout << getObjectOutcome.GetError().GetExceptionName() <<
              " " << getObjectOutcome.GetError().GetMessage() << std::endl;
  }
}

TEST_F(FooTest, DeleteObjectRequest) {
  DeleteObjectRequest request;
  request.WithBucket("test1").WithKey("object1");
  auto deleteObjectOutcome = pclient->DeleteObject(request);
  if (deleteObjectOutcome.IsSuccess()) {
    std::cout << "Successfully" << std::endl;
  }
}

TEST_F(FooTest, DeleteBucketRequest) {
  DeleteBucketRequest request;
  request.SetBucket("test1");
  auto deleteBucketOutcome = pclient->DeleteBucket(request);
  if (deleteBucketOutcome.IsSuccess()) {
    std::cout << "Successfully" << std::endl;
  } else {
    std::cout << deleteBucketOutcome.GetError().GetExceptionName() <<
              " " << deleteBucketOutcome.GetError().GetMessage() << std::endl;
  }
}

int main(int argc,char **argv) {
  Aws::Utils::Logging::InitializeAWSLogging(
    Aws::MakeShared<Aws::Utils::Logging::ConsoleLogSystem>(
      "RunUnitTests", Aws::Utils::Logging::LogLevel::Debug));
  testing::InitGoogleTest(&argc, argv);
  int exitCode = RUN_ALL_TESTS();
  Aws::Utils::Logging::ShutdownAWSLogging();
  return exitCode;
}