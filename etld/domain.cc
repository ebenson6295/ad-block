/* Copyright (c) 2015 Brian R. Bondy. Distributed under the MPL2 license.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <string>
#include "./domain.h"

namespace Brave {
namespace eTLD {

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

std::string Domain::ToString() const {
  std::stringstream asString;
  int num_labels = labels_.size();
  int i = 0;
  for (auto &&str : labels_) {
    asString << str;
    if (i != num_labels - 1) {
      asString << ".";
    }
  }
  return asString.str();
}

}
}