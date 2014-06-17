/**
 * @file
 *
 * @date Jun 13, 2014
 * @author Anton Bondarev
 */

#include <regex.h>

#include <embox/test.h>

EMBOX_TEST_SUITE("regex");


TEST_CASE(" The structure type regex_t contains at least the following member:"
		"size_t    re_nsub") {
	regex_t regex;

	regex.re_nsub = 0;

	test_assert_zero(regex.re_nsub);
}

TEST_CASE("match") {
	regex_t regex;
	int ret;

	ret = regcomp(&regex, "^a[[:alnum:]]", 0);
	test_assert_zero(ret);
}

TEST_CASE("match") {
	regex_t regex;
	regmatch_t match[10];
	int ret;

	ret = regcomp(&regex, "k", 0);
	test_assert_zero(ret);
	test_assert_zero(regexec(&regex, "kk#ab-kF", 10, match, 0));
}
