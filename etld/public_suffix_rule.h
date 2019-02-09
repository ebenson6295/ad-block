/* Copyright (c) 2019 The Brave Software Team. Distributed under the MPL2
 * license. This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef ETLD_PUBLIC_SUFFIX_RULE_H_
#define ETLD_PUBLIC_SUFFIX_RULE_H_

#include <string>
#include <vector>
#include <exception>
#include "./domain.h"
#include "./types.h"

namespace Brave {
namespace eTLD {

class PublicSuffixRuleInputException : public std::exception {
 public:
  explicit PublicSuffixRuleInputException(const char * message) :
    msg_(message) {}
  explicit PublicSuffixRuleInputException(const std::string &message) :
    msg_(message) {}
  virtual const char* what() const throw() {
    return msg_.c_str();
  }
  virtual ~PublicSuffixRuleInputException() throw() {}

 protected:
  std::string msg_;
};

class PublicSuffixRule {
 public:
  PublicSuffixRule() {}
  explicit PublicSuffixRule(const std::string &rule_text);
  explicit PublicSuffixRule(std::vector<Label> labels, bool is_exception,
      bool is_wildcard) :
    labels_(labels),
    is_exception_(is_exception),
    is_wildcard_(is_wildcard) {}

  bool Equals(const PublicSuffixRule &rule) const;
  bool Matches(const Domain &domain) const;
  DomainInfo Apply(const Domain &domain) const;
  std::string ToString() const;
  std::string DomainString() const;

  std::vector<Label> Labels() const {
    return labels_;
  }
  std::vector<Label>::size_type Length() const {
    return labels_.size();
  }
  bool IsException() const {
    return is_exception_;
  }
  bool IsWildcard() const {
    return is_wildcard_;
  }

 protected:
  std::vector<Label> labels_;
  bool is_exception_ = false;
  bool is_wildcard_ = false;
};

}  // namespace eTLD
}  // namespace Brave

#endif  // ETLD_PUBLIC_SUFFIX_RULE_H_
