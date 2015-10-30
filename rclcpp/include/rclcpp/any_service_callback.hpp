// Copyright 2015 Open Source Robotics Foundation, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef RCLCPP_RCLCPP_ANY_SERVICE_CALLBACK_HPP_
#define RCLCPP_RCLCPP_ANY_SERVICE_CALLBACK_HPP_

#include <rclcpp/function_traits.hpp>

#include <functional>
#include <memory>
#include <type_traits>

#include <rmw/types.h>

namespace rclcpp
{

namespace any_service_callback
{

template<typename ServiceT>
class AnyServiceCallback
{
private:
  using SharedPtrCallback = std::function<void(
        const std::shared_ptr<typename ServiceT::Request>,
        std::shared_ptr<typename ServiceT::Response>
      )>;
  using SharedPtrWithRequestHeaderCallback = std::function<void(
        const std::shared_ptr<rmw_request_id_t>,
        const std::shared_ptr<typename ServiceT::Request>,
        std::shared_ptr<typename ServiceT::Response>
      )>;

  SharedPtrCallback shared_ptr_callback_;
  SharedPtrWithRequestHeaderCallback shared_ptr_with_request_header_callback_;

public:
  AnyServiceCallback()
  : shared_ptr_callback_(nullptr), shared_ptr_with_request_header_callback_(nullptr)
  {}

  AnyServiceCallback(const AnyServiceCallback &) = default;

  template<
    typename CallbackT,
    typename std::enable_if<
      rclcpp::same_arguments<
        CallbackT,
        SharedPtrCallback
      >::value
    >::type * = nullptr
  >
  void set(CallbackT callback)
  {
    shared_ptr_callback_ = callback;
  }

  template<
    typename CallbackT,
    typename std::enable_if<
      rclcpp::same_arguments<
        CallbackT,
        SharedPtrWithRequestHeaderCallback
      >::value
    >::type * = nullptr
  >
  void set(CallbackT callback)
  {
    shared_ptr_with_request_header_callback_ = callback;
  }

  void dispatch(
    std::shared_ptr<rmw_request_id_t> request_header,
    std::shared_ptr<typename ServiceT::Request> request,
    std::shared_ptr<typename ServiceT::Response> response)
  {
    if (shared_ptr_callback_ != nullptr) {
      (void)request_header;
      shared_ptr_callback_(request, response);
    } else if (shared_ptr_with_request_header_callback_ != nullptr) {
      shared_ptr_with_request_header_callback_(request_header, request, response);
    } else {
      throw std::runtime_error("unexpected request without any callback set");
    }
  }
};

} /* namespace any_service_callback */
} /* namespace rclcpp */

#endif /* RCLCPP_RCLCPP_ANY_SERVICE_CALLBACK_HPP_ */