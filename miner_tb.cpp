#include "miner.h"
#include <cstring>
#include <iostream>
#include <cstdlib>
#include <algorithm>
#include <string>
#include <string>

#define N_HEADERS 3

CCS_MAIN(int argc, char *argv[]){

	// const char* headers[N_HEADERS] = {
	// 	"0000002066720b99e07d284bd4fe67ff8c49a5db1dd8514fcdab610000000000000000007829844f4c3a41a537b3131ca992643eaa9d093b2383e4cdc060ad7dc548118751eb505ac1910018de19b302",
	// 	"0100000081cd02ab7e569e8bcd9317e2fe99f2de44d49ab2b8851ba4a308000000000000e320b6c2fffc8d750423db8b1eb942ae710e951ed797f7affc8892b0f1fc122bc7f5d74df2b9441a42a14695",
	// 	"04008020b2d01a99dd2983494e81c5776aeb4890542cdc941e9d05000000000000000000c6177ff94b70386f596fb2bc7350523d93fe34dc91290f047babb897bf1844a9ac4e6f6108040e179aa59dbb"
	// };

	const char* headers[N_HEADERS] = {
		"0000002066720b99e07d284bd4fe67ff8c49a5db1dd8514fcdab610000000000000000007829844f4c3a41a537b3131ca992643eaa9d093b2383e4cdc060ad7dc548118751eb505ac19100187b19b302",
		"0100000081cd02ab7e569e8bcd9317e2fe99f2de44d49ab2b8851ba4a308000000000000e320b6c2fffc8d750423db8b1eb942ae710e951ed797f7affc8892b0f1fc122bc7f5d74df2b9441adfa04695",
		"04008020b2d01a99dd2983494e81c5776aeb4890542cdc941e9d05000000000000000000c6177ff94b70386f596fb2bc7350523d93fe34dc91290f047babb897bf1844a9ac4e6f6108040e1737a59dbb"
	};

	std::string answers[N_HEADERS] = {
		"a9e41527e7613422b75f8d58af24c425fb6365dc2bcf20000000000000000000",
		"1dbd981fe6985776b644b173a4d0385ddc1aa2a829688d1e0000000000000000",
		"faffe757431d5f7e87d185646128c0af579757fdf3700a000000000000000000"
	};

	bool pass = true;
	header_t header;
	u256_t result;
	for (int i = 0; i < N_HEADERS; i++) {
		header.bit_fill_hex(headers[i]);
		result = CCS_DESIGN(sha256) (header);
		std::string res_string = result.to_string(AC_HEX, false, true);
		res_string = res_string.substr(2);
		transform(res_string.begin(), res_string.end(), res_string.begin(), ::tolower);
		std::cout << "Calculated:\t" << res_string << std::endl;
		std::cout << "Answer:\t\t" << answers[i] << std::endl;
		bool check = !res_string.compare(answers[i]);
		std::cout << "Pass or Not : " << check << std::endl << std::endl;
		pass &= check;
	}

	if (!pass){
		std::cout << "Test Fail!" << std::endl;
	} else{
		std::cout << "Test Pass!" << std::endl;
	}
	
	CCS_RETURN(0);

}
