#include "../common.h"

/**
 * Perform tests on the JSON implementation.
 *   &returns: Success flag.
 */
bool test_json(void)
{
	bool suc = true;

	{
		struct json_t *json;

		suc &= chkbool(json_parse_str(&json, "12"));
		if(json != NULL) {
			unsigned int num;

			suc &= chkbool(json_getf(json, "u", &num));
			suc &= (num == 12);

			json_delete(json);
		}

		suc &= chkbool(json_parse_str(&json, "32"));
		if(json != NULL) {
			uint16_t num;

			suc &= chkbool(json_getf(json, "u16", &num));
			suc &= (num == 32);

			json_delete(json);
		}

		suc &= chkbool(json_parse_str(&json, "{a:2,foo:\"bar\",c:{d:12}}"));
		if(json != NULL) {
			unsigned int num[2];
			const char *str = NULL;

			suc &= chkbool(json_getf(json, "{a:u,foo:s,c:{d:u$}$}", &num[0], &str, &num[1]));
			suc &= ((num[0] == 2) && (num[1] == 12) && (str != NULL) && (strcmp(str, "bar") == 0));

			json_delete(json);
		}

		suc &= chkbool(json_parse_str(&json, "{foo:true,bar:false}"));
		if(json != NULL) {
			bool foo, bar;

			suc &= chkbool(json_getf(json, "{foo:b,bar:b$}", &foo, &bar));
			suc &= ((foo == true) && (bar == false));

			json_delete(json);
		}
	}

	return suc;
}
