//
// offset_fetch_response.h
// -----------------------
//
// Copyright (c) 2015 Daniel Joos
//
// Distributed under MIT license. (See file LICENSE)
//

#ifndef OFFSET_FETCH_RESPONSE_H_0D96F788_105E_410C_8BE6_218FA04045AC
#define OFFSET_FETCH_RESPONSE_H_0D96F788_105E_410C_8BE6_218FA04045AC

#include <vector>
#include <libkafka_asio/response.h>
#include <libkafka_asio/detail/topic_partition_map.h>

namespace libkafka_asio
{

class MutableOffsetFetchResponse;

// Kafka Offset Commit/Fetch API response implementation:
// Offset fetch response
class OffsetFetchResponse :
  public Response<OffsetFetchResponse>
{
  friend class MutableOffsetFetchResponse;

  struct TopicPartitionProperties
  {
    Int64 offset;
    String metadata;
    Int16 error_code;
  };

public:
  typedef detail::TopicPartitionMap<TopicPartitionProperties> Topic;
  typedef Topic::MapType TopicMap;

  const TopicMap& topics() const;

private:
  TopicMap topics_;
};

class MutableOffsetFetchResponse :
  public MutableResponse<OffsetFetchResponse>
{
public:
  OffsetFetchResponse::TopicMap& mutable_topics();
};

}  // namespace libkafka_asio

#include <libkafka_asio/impl/offset_fetch_response.h>

#endif  // OFFSET_FETCH_RESPONSE_H_0D96F788_105E_410C_8BE6_218FA04045AC
