/* Copyright (c) 2015 Brian R. Bondy. Distributed under the MPL2 license.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_ETLD_DOMAIN_H_
#define BRAVE_ETLD_DOMAIN_H_

#include <string>
#include <sstream>
#include <vector>
#include "./types.h"

namespace Brave {
namespace eTLD {

class Domain {
  public:
    const Label& operator[](std::size_t idx) const {
      return labels_[idx];
    }

    Domain(const std::string &domain);

    int Length() const {
      return labels_.size();
    }

    std::string ToString() const;

  protected:
    std::vector<Label> labels_;
};

}
}

#endif