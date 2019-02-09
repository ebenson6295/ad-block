#!/usr/bin/env node

/* Copyright (c) 2019 The Brave Software Team. Distributed under the MPL2
 * license. This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/**
 * This code parses the public suffix rule list into a header file of
 * rules that gets build into the ad-block library.
 */

const fsLib = require("fs");
const pathLib = require("path");

const ruleTextPath = pathLib.join(__dirname, "public_suffix_list.dat");
const ruleText = fsLib.readFileSync(ruleTextPath, "utf8");

const templatePath = pathLib.join(__dirname, "public_suffix_list.h.template");
const templateText = fsLib.readFileSync(templatePath, "utf8");
const rules = [];

const toPublicSuffixRuleSerializedString = (isWildcard, isException, labels) => {
  const constructorArgs = [];
  constructorArgs.push(isWildcard ? "true" : "false");
  constructorArgs.push(isException ? "true" : "false");

  const wrappedLabels = labels.map(JSON.stringify);
  constructorArgs.push("{" + wrappedLabels.join(", ") + "}");
  return "{" + constructorArgs.join(", ") + "}";
}

for (const line of ruleText.split("\n")) {
  let isWildcard = false;
  let isException = false;
  let trimmedLine = line.trim();
  if (trimmedLine.length === 0) {
    continue;
  }

  // Check to see if this is a comment line.  If so, process no further.
  if (trimmedLine.indexOf("//") == 0) {
    continue;
  }

  const firstChar = trimmedLine[0];
  switch (firstChar) {
    case "!":
      trimmedLine = trimmedLine.slice(1);
      isException = true;
      break;

    case "*":
      isWildcard = true;
      break;

    default:
      break;
  }

  const lineUntilWhiteSpace = trimmedLine.split(" ")[0];
  const ruleLabels = lineUntilWhiteSpace.split(".");

  const ruleAsString = toPublicSuffixRuleSerializedString(isWildcard, isException, ruleLabels);
  rules.push(ruleAsString);
}

const serializedString = rules.join(",\n");
const replaceArg = "{" + serializedString + "}";
const finalCodeText = templateText.replace("{contents}", replaceArg);

const generatedFilePath = pathLib.join(__dirname, "public_suffix_list.h");
fsLib.writeFileSync(generatedFilePath, finalCodeText, "utf8");
