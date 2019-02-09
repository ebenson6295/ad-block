/* Copyright (c) 2019 The Brave Software Team. Distributed under the MPL2
 * license. This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <string>
#include <iostream>
#include <vector>
#include <set>
#include <fstream>
#include "./CppUnitLite/TestHarness.h"
#include "./CppUnitLite/Test.h"
#include "./etld/types.h"
#include "./etld/matcher.h"
#include "./etld/shared_matcher.h"
#include "./etld/domain.h"
#include "./etld/public_suffix_rule.h"
#include "./etld/public_suffix_rule_set.h"

using std::string;
using std::cout;
using std::endl;


using brave_etld::PublicSuffixTextLineType;
using brave_etld::PublicSuffixRuleInputException;
using brave_etld::PublicSuffixRuleSetMatchResult;
using brave_etld::PublicSuffixRule;
using brave_etld::PublicSuffixRuleSerialized;
using brave_etld::PublicSuffixRuleSet;
using brave_etld::Domain;
using brave_etld::Matcher;
using brave_etld::SharedMatcher;
using brave_etld::DomainInfo;

bool testMatchingDomainRules(const PublicSuffixRuleSerialized &serialized_rule, const string &domain_str,
    bool should_match) {
  Domain domain(domain_str);
  PublicSuffixRule rule(serialized_rule);

  bool does_match = rule.Matches(domain);
  if (should_match != does_match) {
    string desc(should_match
      ? "should have, but didn't match "
      : "didn't, but should have matched ");
    cout << "Domain: '" << domain.ToString() << "' "
         << desc
         << "Rule: '" << rule.ToString() << "'" << endl;
    return false;
  }

  return true;
}

bool testPublicSuffixRuleSetMatch(const std::vector<PublicSuffixRuleSerialized> &rules,
    const string &domain_str, const string &expected_match) {
  PublicSuffixRuleSet rule_set;
  PublicSuffixRule rule;
  for (auto &elm : rules) {
    rule = PublicSuffixRule(elm);
    rule_set.AddRule(rule);
  }

  Domain test_domain(domain_str);
  PublicSuffixRuleSetMatchResult match_result = rule_set.Match(test_domain);
  string result_string = match_result.rule.DomainString();
  if (result_string != expected_match) {
    cout << "Expected rule: '" << expected_match << "' "
         << "to match '" << domain_str << "' "
         << "but received '" << result_string << "'" << endl;
    return false;
  }

  return true;
}

bool testPublicSuffixRuleTextApply(const PublicSuffixRuleSerialized &serialized_rule,
    const string &domain_str, const DomainInfo &expected) {
  PublicSuffixRule rule(serialized_rule);
  Domain domain(domain_str);
  DomainInfo extracted = rule.Apply(domain);
  if (extracted.tld != expected.tld) {
    cout << "Expected tld of '" << expected.tld
         << "' when applying rule '"
         << rule_str << "' to domain '" << domain_str
         << "', but received tld '" << extracted.tld << "'" << endl;
    return false;
  }

  if (extracted.domain != expected.domain) {
    cout << "Expected domain of '" << expected.domain
         << "' when applying rule '"
         << rule_str << "' to domain '" << domain_str
         << "', but received domain '"
         << extracted.domain << "'" << endl;
    return false;
  }

  if (extracted.subdomain != expected.subdomain) {
    cout << "Expected subdomain of '" << expected.subdomain
         << "' when applying rule '"
         << rule_str << "' to domain '" << domain_str
         << "', but received subdomain '"
         << extracted.subdomain << "'" << endl;
    return false;
  }

  return true;
}

bool testPublicSuffixListApply(const string &domain_str, const DomainInfo &expected) {
  Matcher matcher = SharedMatcher.GetInstance.GetMatcher();
  DomainInfo extracted = matcher.Match(Domain(domain_str));

  if (extracted.tld != expected.tld) {
    cout << "Expected tld of '" << expected.tld << " for domain '"
         << domain_str << "', but received tld '"
         << extracted.tld << "'" << endl;
    return false;
  }

  if (extracted.domain != expected.domain) {
    cout << "Expected domain of '" << expected.domain << " for domain '"
         << domain_str << "', but received domain '"
         << extracted.domain << "'" << endl;
    return false;
  }

  if (extracted.subdomain != expected.subdomain) {
    cout << "Expected subdomain of '" << expected.subdomain << " for domain '"
         << domain_str << "', but received subdomain '"
         << extracted.subdomain << "'" << endl;
    return false;
  }

  return true;
}

TEST(eTLDMatchTests, basic) {
  CHECK(testMatchingDomainRules(PublicSuffixRuleSerialized(false, false, {"com"}), "com", true));
  CHECK(testMatchingDomainRules(PublicSuffixRuleSerialized(false, false, {"fp", "com"}), "com", false));
  CHECK(testMatchingDomainRules(PublicSuffixRuleSerialized(false, false, {"com"}), "fp.com", true));
  CHECK(testMatchingDomainRules(PublicSuffixRuleSerialized(true, false, {"*", "hokkaido", "jp"}), "bar.hokkaido.jp", true));
  CHECK(testMatchingDomainRules(PublicSuffixRuleSerialized(false, true, {"pref", "hokkaido", "jp"}), "pref.hokkaido.jp", true));
  CHECK(testMatchingDomainRules(PublicSuffixRuleSerialized(true, false, {"*", "tokyo", "jp"}), "tokyo.jp", false));
  CHECK(testMatchingDomainRules(PublicSuffixRuleSerialized(true, false, {"*", "tokyo", "jp"}), "other.tokyo.jp", true));
  CHECK(testMatchingDomainRules(PublicSuffixRuleSerialized(false, false, {"com"}), "foo.com", true));
}

TEST(eTLDPublicSuffixRuleSetMatchTests, basic) {
  CHECK(testPublicSuffixRuleSetMatch(
    {
      PublicSuffixRuleSerialized(false, false, {"com"}),
      PublicSuffixRuleSerialized(true, false, {"*", "jp"}),
      PublicSuffixRuleSerialized(true, false, {"*", "hokkaido", "jp"})
    },
    "foo.com",
    "com"));
  CHECK(testPublicSuffixRuleSetMatch(
    {
      PublicSuffixRuleSerialized(false, false, {"com"}),
      PublicSuffixRuleSerialized(true, false, {"*", "jp"}),
      PublicSuffixRuleSerialized(true, false, {"*", "hokkaido", "jp"})
    },
    "hokkaido.jp",
    "*.jp"));
  CHECK(testPublicSuffixRuleSetMatch(
    {
      PublicSuffixRuleSerialized(false, false, {"com"}),
      PublicSuffixRuleSerialized(true, false, {"*", "jp"}),
      PublicSuffixRuleSerialized(true, false, {"*", "hokkaido", "jp"})
    },
    "pete.hokkaido.jp",
    "*.hokkaido.jp"));
  CHECK(testPublicSuffixRuleSetMatch(
    {
      PublicSuffixRuleSerialized(false, false, {"com"}),
      PublicSuffixRuleSerialized(true, false, {"*", "jp"}),
      PublicSuffixRuleSerialized(true, false, {"*", "hokkaido", "jp"})
    },
    "horse.shoes",
    ""));
}

TEST(eTLDPublicSuffixRuleApplyTestTests, basic) {
  DomainInfo test_one_expected_match = {"example.jp", "shoes", "pete"};
  CHECK(testPublicSuffixRuleTextApply(
    PublicSuffixRuleSerialized(true, false, {"*", "jp"}),
    "pete.shoes.example.jp",
    test_one_expected_match));

  DomainInfo test_two_expected_match = {"horse", "the", "we.love"};
  CHECK(testPublicSuffixRuleTextApply(
    PublicSuffixRuleSerialized(false, false, {"horse"}),
    "we.love.the.horse", test_two_expected_match));

  DomainInfo test_three_expected_match = {"tokyo.jp", "metro", "slate"};
  CHECK(testPublicSuffixRuleTextApply(
    PublicSuffixRuleSerialized(false, true, {"!metro", "tokyo", "jp"}),
    "slate.metro.tokyo.jp", test_three_expected_match));
}

TEST(eTLDPublicSuffixRuleApplyFileTests, basic) {
  DomainInfo file_test_one = {"com", "google", "www"};
  CHECK(testPublicSuffixListApply(
    "www.google.com",
    file_test_one));

  DomainInfo file_test_two = {"co.uk", "google", ""};
  CHECK(testPublicSuffixListApply(
    "google.co.uk",
    file_test_two));

  // Tests taken from https://www.npmjs.com/package/tldts
  DomainInfo file_test_three = {"s3.amazonaws.com", "spark-public", ""};
  CHECK(testPublicSuffixListApply(
    "spark-public.s3.amazonaws.com",
    file_test_three));

  DomainInfo file_test_four = {"org", "writethedocs", "www"};
  CHECK(testPublicSuffixListApply(
    "www.writethedocs.org",
    file_test_four));

  // Tests taken from https://www.npmjs.com/package/tld
  DomainInfo file_test_five = {"bar.sch.uk", "foo", "www"};
  CHECK(testPublicSuffixListApply(
    "www.foo.bar.sch.uk",
    file_test_five));

  // Tests taken from https://www.npmjs.com/package/parse-domain
  DomainInfo file_test_six = {"co.uk", "example", "some.subdomain"};
  CHECK(testPublicSuffixListApply(
    "some.subdomain.example.co.uk",
    file_test_six));

  // Tests adapted from
  // https://raw.githubusercontent.com/publicsuffix/list/master/tests/test_psl.txt
  struct DomainInfoTestCase {
    string input;
    DomainInfo expected;
  };
  DomainInfoTestCase file_test_cases[] = {
    {"com", {"com", "", ""}},
    {"example.com", {"com", "example", ""}},
    {"www.example.com", {"com", "example", "www"}},
    {"example", {"example", "", ""}},
    {"example.example", {"example", "example", ""}},
    {"b.example.example", {"example", "example", "b"}},
    {"a.b.example.example", {"example", "example", "a.b"}},
    {"biz", {"biz", "", ""}},
    {"domain.biz", {"biz", "domain", ""}},
    {"b.domain.biz", {"biz", "domain", "b"}},
    {"uk.com", {"uk.com", "", ""}},
    {"example.uk.com", {"uk.com", "example", ""}},
    {"b.example.uk.com", {"uk.com", "example", "b"}},
    {"a.b.example.uk.com", {"uk.com", "example", "a.b"}},
    {"jp", {"jp", "", ""}},
    {"test.jp", {"jp", "test", ""}},
    {"www.test.jp", {"jp", "test", "www"}},
    {"ac.jp", {"ac.jp", "", ""}},
    {"test.ac.jp", {"ac.jp", "test", ""}},
    {"www.test.ac.jp", {"ac.jp", "test", "www"}},
    {"kyoto.jp", {"kyoto.jp", "", ""}},
    {"test.kyoto.jp", {"kyoto.jp", "test", ""}},
    {"ide.kyoto.jp", {"ide.kyoto.jp", "", ""}},
    {"b.ide.kyoto.jp", {"ide.kyoto.jp", "b", ""}},
    {"a.b.ide.kyoto.jp", {"ide.kyoto.jp", "b", "a"}},
    {"c.kobe.jp", {"c.kobe.jp", "", ""}},
    {"b.c.kobe.jp", {"c.kobe.jp", "b", ""}},
    {"a.b.c.kobe.jp", {"c.kobe.jp", "b", "a"}},
    {"city.kobe.jp", {"kobe.jp", "city", ""}},
    {"www.city.kobe.jp", {"kobe.jp", "city", "www"}},
    {"ck", {"ck", "", ""}},
    {"test.ck", {"test.ck", "", ""}},
    {"b.test.ck", {"test.ck", "b", ""}},
    {"a.b.test.ck", {"test.ck", "b", "a"}},
    {"www.ck", {"ck", "www", ""}},
    {"www.www.ck", {"ck", "www", "www"}},
    {"us", {"us", "", ""}},
    {"test.us", {"us", "test", ""}},
    {"www.test.us", {"us", "test", "www"}},
    {"ak.us", {"ak.us", "", ""}},
    {"test.ak.us", {"ak.us", "test", ""}},
    {"www.test.ak.us", {"ak.us", "test", "www"}},
    {"k12.ak.us", {"k12.ak.us", "", ""}},
    {"test.k12.ak.us", {"k12.ak.us", "test", ""}},
    {"www.test.k12.ak.us", {"k12.ak.us", "test", "www"}},
    {"食狮.com.cn", {"com.cn", "食狮", ""}},
    {"食狮.公司.cn", {"公司.cn", "食狮", ""}},
    {"sushi.公司.cn", {"公司.cn", "sushi", ""}},
    {"公司.cn", {"公司.cn", "", ""}},
    {"食狮.中国", {"中国", "食狮", ""}},
    {"shishi.中国", {"中国", "shishi", ""}},
    {"中国", {"中国", "", ""}},
  };
  for (auto &elm : file_test_cases) {
    CHECK(testPublicSuffixRuleFileApply(
      elm.input,
      elm.expected));
  }
}
