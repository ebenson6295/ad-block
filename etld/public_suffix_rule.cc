/* Copyright (c) 2015 Brian R. Bondy. Distributed under the MPL2 license.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <string>
#include "./domain.h"
#include "./public_suffix_rule.h"

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
    throw PublicSuffixRuleInputException("Cannot create PublicSuffixRule from an empty string");
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
      throw PublicSuffixRuleInputException("Rules cannot start with '/'");
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
      throw PublicSuffixRuleInputException("Rules cannot contain adjectent delimitors");
    }
    labels_.push_back(current_label);
    previous = current + 1;
    current = trimmed_rule_text.find(".", previous);
  }

  // If don't include any trailing whitespace, if there is any.
  current_label = trimmed_rule_text.substr(previous, current - previous);
  if (current_label == "") {
    throw PublicSuffixRuleInputException("Rules cannot end with a delimiter");
  }

  labels_.push_back(current_label);
}

bool PublicSuffixRule::Equals(const PublicSuffixRule &rule) const {
  return (
    labels_ == rule.Labels() &&
    is_exception_ == rule.IsException() &&
    is_wildcard_ == rule.IsWildcard()
  );
}

// Implements matching algoritms described here
//   https://publicsuffix.org/list/
bool PublicSuffixRule::Matches(const Domain &domain) const {
  // When the domain and rule are split into corresponding labels, that the
  // domain contains as many or more labels than the rule.
  std::vector<Label> labels(Labels());
  int num_labels = labels.size();
  if (num_labels > domain.Length()) {
    return false;
  }

  // Beginning with the right-most labels of both the domain and the rule, and
  // continuing for all labels in the rule, one finds that for every pair,
  // either they are identical, or that the label from the rule is "*".
  for (int i = num_labels - 1; i >= 0; i -= 1) {
    std::string rule_label = labels[i];
    if (rule_label == "*") {
      continue;
    }

    if (labels[i] != domain[i]) {
      return false;
    }
  }

  return true;
}

std::string PublicSuffixRule::ToString() const {
  std::stringstream asString;
  asString << "labels: [";
  for (auto &&str : labels_) {
    asString << str << ", ";
  }
  asString << "] ";

  asString << "is exception: " << (is_exception_ ? "true" : "false") << " ";
  asString << "is wildcard: " << ( is_wildcard_ ? "true" : "false");

  return asString.str();
}


}
}