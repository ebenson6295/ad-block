/* Copyright (c) 2015 Brian R. Bondy. Distributed under the MPL2 license.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <string>
#include <iostream>
#include <vector>
#include <set>
#include "./CppUnitLite/TestHarness.h"
#include "./CppUnitLite/Test.h"
#include "./etld/parser.h"
#include "./etld/domain.h"
#include "./etld/public_suffix_rule.h"

using std::string;
using std::cout;
using std::endl;

using Brave::eTLD::PublicSuffixTextLineType;
using Brave::eTLD::PublicSuffixTextLineParseResult;
using Brave::eTLD::PublicSuffixRuleInputException;
using Brave::eTLD::PublicSuffixRule;

bool testParsePublicSuffixTestLineNoOps(const std::string &line, PublicSuffixTextLineType expectedType) {
  PublicSuffixTextLineParseResult result = Brave::eTLD::parse_rule_line(line);
  if (result.type != expectedType) {
    cout << "Received unexpected parse result for " << line << endl;
    return false;
  }
  
  return true;
}

bool testParsePublicSuffixTestLineError(const std::string &line, const std::string &expected_error) {
  try {
    PublicSuffixRule rule(line);
  } catch (PublicSuffixRuleInputException error) {
    if (error.what() != expected_error) {
      cout << "Caught an unexpected error when parsing " << line << "\n"
           << "Expected '" << expected_error << "'\n"
           << "Received '" << error.what() << endl;
      return false;
    }
    return true;
  }

  cout << "Expected to receive an error when attempting to parse invalid rule '"
       << line
       << "' but no error was thrown." << endl;
  return false;
}

bool testParsePublicSuffixTest(
    const std::string &line,
    std::vector<std::string> expected_labels,
    bool expected_exception,
    bool expected_wildcard) {
  PublicSuffixRule rule(line);
  PublicSuffixRule currect_rule(expected_labels, expected_exception, expected_wildcard);
  return currect_rule.Equals(rule);
}

bool testParsePublicSuffixTextTest(
    const std::string &text,
    int num_expected_rules,
    int num_expected_exception_rules,
    int num_expected_wildcard_rules,
    int num_expected_whitespace_lines,
    int num_expected_comment_lines,
    int num_expected_invalid_lines) {
  Brave::eTLD::PublicSuffixParseResult result = Brave::eTLD::parse_rule_text(text);

  if (num_expected_rules != result.Rules().size()) {
    cout << "Incorrect number of rules parsed, expected '" << num_expected_rules << "' "
         << "but found '" << result.Rules().size() << "'" << endl;
    return false;
  }

  int num_found_exception_rules = 0;
  int num_found_wildcard_rules = 0;
  for (auto &&elm : result.Rules()) {
    if (elm.IsException()) {
      num_found_exception_rules += 1;
    }
    if (elm.IsWildcard()) {
      num_found_wildcard_rules += 1;
    }
  }

  if (num_found_exception_rules != num_expected_exception_rules) {
    cout << "Found unexpected number of exception rules, expected '" << num_expected_exception_rules << "' "
         << "but found '" << num_found_exception_rules << "'" << endl;
    return false;
  }

  if (num_found_exception_rules != num_expected_exception_rules) {
    cout << "Found unexpected number of exception rules, expected '" << num_expected_exception_rules << "' "
         << "but found '" << num_found_exception_rules << "'" << endl;
    return false;
  }

  if (result.NumWhitespaceLines() != num_expected_whitespace_lines) {
    cout << "Found unexpected number of whitespace lines, expected '" << num_expected_whitespace_lines << "' "
         << "but found '" << result.NumWhitespaceLines() << "'" << endl;
    return false;
  }

  if (result.NumInvalidRules() != num_expected_invalid_lines) {
    cout << "Found unexpected number of invalid rules, expected '" << num_expected_invalid_lines << "' "
         << "but found '" << result.NumInvalidRules() << "'" << endl;
    return false;
  }

  if (result.NumCommentLines() != num_expected_comment_lines) {
    cout << "Found unexpected number of comment lines, expected '" << num_expected_comment_lines << "' "
         << "but found '" << result.NumCommentLines() << "'" << endl;
    return false;
  }

  return true;
}

bool testMatchingDomainRules(const std::string &rule_str, const std::string &domain_str, bool should_match) {
  Brave::eTLD::Domain domain(domain_str);
  Brave::eTLD::PublicSuffixRule rule(rule_str);

  bool does_match = rule.Matches(domain);
  if (should_match != does_match) {
    std::string desc(should_match
      ? "should have, but didn't match "
      : "didn't, but should have matched "
    );
    cout << "Domain: '" << domain.ToString() << "' "
         << desc
         << "Rule: '" << rule.ToString() << "'";
    return false;
  }

  return true;
}

TEST(eTLDParseRulesLine, basic) {
  CHECK(testParsePublicSuffixTestLineNoOps(
      "// this is an option",
      Brave::eTLD::PublicSuffixTextLineTypeComment
    )
  );
  CHECK(testParsePublicSuffixTestLineNoOps(
      "this.is.a.invalid..rule",
      Brave::eTLD::PublicSuffixTextLineTypeInvalidRule
    )
  );
  CHECK(testParsePublicSuffixTestLineNoOps(
      "",
      Brave::eTLD::PublicSuffixTextLineTypeWhitespace
    )
  );

  // Since we read until the first white space, this will be read
  // as "example.", which is invalid because rules cannot hae empty labels
  CHECK(testParsePublicSuffixTestLineError(
      "example. org",
      "Rules cannot end with a delimiter"
    )
  );
  CHECK(testParsePublicSuffixTestLineError(
      "example..org",
      "Rules cannot contain adjectent delimitors"
    )
  );
  CHECK(testParsePublicSuffixTestLineError(
      "example.org.",
      "Rules cannot end with a delimiter"
    )
  );

  // Examples taken from https://publicsuffix.org/list/
  CHECK(testParsePublicSuffixTest(
      "com",
      {"com"},
      false,
      false
    )
  );
  CHECK(testParsePublicSuffixTest(
      "*.jp",
      {"*", "jp"},
      false,
      true
    )
  );
  CHECK(testParsePublicSuffixTest(
      "*.jp",
      {"*", "jp"},
      false,
      true
    )
  );
  CHECK(testParsePublicSuffixTest(
      "*.tokyo.jp",
      {"*", "tokyo", "jp"},
      false,
      true
    )
  );
  CHECK(testParsePublicSuffixTest(
      "!pref.hokkaido.jp",
      {"pref", "hokkaido", "jp"},
      true,
      false
    )
  );
}

TEST(eTLDParseRulesText, basic) {
  // Just retesting the examples from above, to make sure they are parsed
  // correctly when all in a line.
  CHECK(testParsePublicSuffixTextTest(
      "com\n*.jp\n*.jp\n*.tokyo.jp\n!pref.hokkaido.jp",
      5, 1, 3, 0, 0, 0
    )
  );
  // First example from 
  CHECK(testParsePublicSuffixTextTest(
      "// ac : https://en.wikipedia.org/wiki/.ac\nac\ncom.ac\nedu.ac\ngov.ac\nnet.ac\nmil.ac\norg.ac",
      7, 0, 0, 0, 1, 0
    )
  );
  CHECK(testParsePublicSuffixTextTest(
      "// am : https://en.wikipedia.org/wiki/.am\n"
      "am\n"
      "\n"
      "// ao : https://en.wikipedia.org/wiki/.ao \n"
      "// http://www.dns.ao/REGISTR.DOC \n"
      "ao\n"
      "ed.ao\n"
      "gv.ao\n"
      "og.ao\n"
      "co.ao\n"
      "pb.ao\n"
      "it.ao\n",
      8, 0, 0, 1, 3, 0
    )
  );
  CHECK(testParsePublicSuffixTextTest(
      "\n"
      "*.wild.rule\n"
      "!exception.rule \n"
      "// coment line \n"
      "example.org\n"
      "...badrule",
      3, 1, 1, 1, 1, 1
    )
  );
}

TEST(eTLDMatchTests, basic) {
  CHECK(testMatchingDomainRules(
      "com", "com", true
    )
  );
  CHECK(testMatchingDomainRules(
      "fp.com", "com", false
    )
  );
  CHECK(testMatchingDomainRules(
      "*.hokkaido.jp", "bar.hokkaido.jp", true
    )
  );
  CHECK(testMatchingDomainRules(
      "!pref.hokkaido.jp", "pref.hokkaido.jp", true
    )
  );
  CHECK(testMatchingDomainRules(
      "*.tokyo.jp", "tokyo.jp", false
    )
  );
  CHECK(testMatchingDomainRules(
      "*.tokyo.jp", "other.tokyo.jp", true
    )
  );
}