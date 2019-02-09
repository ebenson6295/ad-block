/* Copyright (c) 2019 The Brave Software Team. Distributed under the MPL2
 * license. This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <string>
#include <sstream>
#include "etld/domain.h"

namespace brave_etld {

Domain::Domain(const std::string &string) {
  std::size_t current, previous = 0;
  current = string.find(".");
  if (current == std::string::npos) {
    labels_.push_back(string);
    return;
  }

  while (current != std::string::npos) {
    labels_.push_back(string.substr(previous, current - previous));
    previous = current + 1;
    current = string.find(".", previous);
  }

  if (previous != 0) {
    labels_.push_back(string.substr(previous, current - previous));
  }
}

Domain::Domain(const std::vector<Label> &labels) {
  labels_ = labels;
}

std::string Domain::ToString() const {
  std::stringstream as_string;
  auto num_labels = labels_.size();
  size_t i = 0;
  for (auto &str : labels_) {
    as_string << str;
    if (i != num_labels - 1) {
      as_string << ".";
    }
    i += 1;
  }
  return as_string.str();
}

}  // namespace brave_etld
