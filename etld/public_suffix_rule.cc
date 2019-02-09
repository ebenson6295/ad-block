/* Copyright (c) 2019 The Brave Software Team. Distributed under the MPL2
 * license. This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <string>
#include <sstream>
#include "etld/domain.h"
#include "etld/public_suffix_rule.h"

namespace Brave {
namespace eTLD {

// Cargo-cult-ed from https://codereview.stackexchange.com/questions/40124/trim-white-space-from-string
std::string trim_to_whitespace(std::string const& str) {
  if (str.empty()) {
    return str;
  }

  std::size_t first_scan = str.find_first_of(' ');
  if (first_scan == std::string::npos) {
    return str;
  }

  return str.substr(0, first_scan);
}

PublicSuffixRule::PublicSuffixRule(const std::string &rule_text) {
  std::string trimmed_rule_text(trim_to_whitespace(rule_text));
  if (trimmed_rule_text.length() == 0) {
    throw PublicSuffixRuleInputException(
      "Cannot create PublicSuffixRule from an empty string");
  }

  std::size_t current = 0;
  switch (trimmed_rule_text[0]) {
    case '*':
      is_wildcard_ = true;
      break;

    case '!':
      is_exception_ = true;
      current += 1;
      break;

    case '/':
      throw PublicSuffixRuleInputException(
        "Rules cannot start with '/': " + rule_text);
      break;

    default:
      break;
  }

  std::size_t previous = current;
  current = trimmed_rule_text.find(".", current);
  Label current_label;
  while (current != std::string::npos) {
    current_label = trimmed_rule_text.substr(previous, current - previous);
    if (current_label.length() == 0) {
      throw PublicSuffixRuleInputException(
        "Rules cannot contain adjectent delimitors: " + rule_text);
    }
    labels_.push_back(current_label);
    previous = current + 1;
    current = trimmed_rule_text.find(".", previous);
  }

  // If don't include any trailing whitespace, if there is any.
  current_label = trimmed_rule_text.substr(previous, current - previous);
  if (current_label == "") {
    throw PublicSuffixRuleInputException(
      "Rules cannot end with a delimiter: " + rule_text);
  }

  labels_.push_back(current_label);
}

bool PublicSuffixRule::Equals(const PublicSuffixRule &rule) const {
  return (
    labels_ == rule.Labels() &&
    is_exception_ == rule.IsException() &&
    is_wildcard_ == rule.IsWildcard());
}

// Implements matching algoritms described here
//   https://publicsuffix.org/list/
bool PublicSuffixRule::Matches(const Domain &domain) const {
  // When the domain and rule are split into corresponding labels, that the
  // domain contains as many or more labels than the rule.
  auto num_rule_labels = labels_.size();
  auto num_domain_labels = domain.Length();
  if (num_rule_labels > num_domain_labels) {
    return false;
  }

  // Beginning with the right-most labels of both the domain and the rule, and
  // continuing for all labels in the rule, one finds that for every pair,
  // either they are identical, or that the label from the rule is "*".
  for (size_t i = 0; i < num_rule_labels; i += 1) {
    Label rule_label = labels_[num_rule_labels - i - 1];
    Label domain_label = domain.Get(num_domain_labels - i - 1);

    if (rule_label == "*") {
      continue;
    }

    if (rule_label != domain_label) {
      return false;
    }
  }

  return true;
}

DomainInfo PublicSuffixRule::Apply(const Domain &domain) const {
  DomainInfo domain_info;

  auto domain_len = domain.Length();
  auto rule_len = Length();
  if (IsException()) {
    rule_len -= 1;
  }

  auto tld_seg_len = rule_len;
  std::stringstream tld_seg;

  auto domain_seg_len = (tld_seg_len == domain_len) ? 0 : 1;
  std::stringstream domain_seg;

  auto sub_domain_seg_len = domain_len - domain_seg_len - tld_seg_len;
  std::stringstream sub_domain_seg;

  Label domain_label;
  for (size_t i = 0; i < domain_len; i += 1) {
    domain_label = domain.Get(i);

    if (sub_domain_seg_len > 0) {
      sub_domain_seg_len -= 1;
      sub_domain_seg << domain_label;
      if (sub_domain_seg_len != 0) {
        sub_domain_seg << ".";
      }
      continue;
    }

    if (domain_seg_len > 0) {
      domain_seg_len -= 1;
      domain_seg << domain_label;
      if (domain_seg_len != 0) {
        domain_seg << ".";
      }
      continue;
    }

    tld_seg_len -= 1;
    tld_seg << domain_label;
    if (tld_seg_len != 0) {
      tld_seg << ".";
    }
  }

  domain_info.tld = tld_seg.str();
  domain_info.domain = domain_seg.str();
  domain_info.subdomain = sub_domain_seg.str();
  return domain_info;
}

std::string PublicSuffixRule::DomainString() const {
  auto last_label_index = labels_.size() - 1;
  size_t label_index = 0;
  std::stringstream as_string;
  for (auto &elm : labels_) {
    as_string << elm;
    if (label_index != last_label_index) {
      as_string << ".";
    }
    label_index += 1;
  }

  return as_string.str();
}

std::string PublicSuffixRule::ToString() const {
  std::stringstream as_string;
  as_string << "labels: [" << DomainString() << "] ";
  as_string << "is exception: " << (is_exception_ ? "true" : "false") << " ";
  as_string << "is wildcard: " << (is_wildcard_ ? "true" : "false");
  return as_string.str();
}

}  // namespace eTLD
}  // namespace Brave
