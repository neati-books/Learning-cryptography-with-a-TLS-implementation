#include"sha1.h"
#include"mpz.h"
#include"pss.h"
#include"util/log.h"
using namespace std;


template std::vector<uint8_t> mgf1<SHA2>(vector<uint8_t> m, int len);
template std::vector<uint8_t> pss_encode<SHA2>(vector<uint8_t> v, int len);
template bool pss_verify<SHA2>(vector<uint8_t> msg, vector<uint8_t> em);

template<class HASH> vector<uint8_t> mgf1(vector<uint8_t> seed, int len)
{//mask generation function mgf1(seed, seed size, mask length)
	HASH sha;
	vector<uint8_t> T;
	for(int i=0; i<4; i++) seed.push_back(0);
	for(int i=0; T.size() < len; i++) {
		mpz2bnd(i, seed.end() - 4, seed.end());
		auto a = sha.hash(seed.begin(), seed.end());
		T.insert(T.end(), a.begin(), a.end());
	}
	T.resize(len);
	return T;
}

template<class HASH> vector<uint8_t> pss_encode(vector<uint8_t> v, int len)
{//salt size should be same as output size
	HASH sha;
	auto a = sha.hash(v.begin(), v.end());
	uint8_t salt[HASH::output_size];
	mpz2bnd(random_prime(HASH::output_size), salt, salt + HASH::output_size);
	vector<uint8_t> M_dash{0, 0, 0, 0, 0, 0, 0, 0};
	M_dash.insert(M_dash.end(), a.begin(), a.end());
	M_dash.insert(M_dash.end(), salt, salt + HASH::output_size);
	a = sha.hash(M_dash.begin(), M_dash.end());//H

	int db_size = len - HASH::output_size - 1;
	uint8_t DB[db_size] = {0, };
	memcpy(DB + db_size - HASH::output_size, salt, HASH::output_size);//8
	DB[db_size - HASH::output_size - 1] = 1;//DB = 000000|1|salt
	auto dbMask = mgf1<HASH>({a.begin(), a.end()}, db_size);
	LOGD << hexprint("dbmask", dbMask) << endl;
	for(int i=0; i<db_size; i++) dbMask[i] ^= DB[i];//maskedDB
	dbMask.insert(dbMask.end(), a.begin(), a.end());
	dbMask.push_back(0xbc);
	return dbMask;//maskedDB|H|0xbc
}

//template<class HASH>
//mpz_class PSS<HASH, SLen>::sign(uint8_t *p, int sz) 
//{
//	auto v = emsa_pss_encode(p, sz, emLen_);
//	return rsa_.sign(bnd2mpz(v.begin(), v.end()));
//}
//
template<class HASH> bool pss_verify(vector<uint8_t> msg, vector<uint8_t> em)
{
	HASH sha;
	auto mHash = sha.hash(msg.begin(), msg.end());
	if(em.back() != 0xbc) return false;
	else em.pop_back();
	auto dbMask = mgf1<HASH>({em.end() - HASH::output_size, em.end()}, em.size()-HASH::output_size);
	LOGD << hexprint("dbmask", dbMask) << endl;
	for(int i=0; i<dbMask.size(); i++) dbMask[i] ^= em[i];//DB
	vector<uint8_t> M_dash = {0,0,0,0,0,0,0,0};
	M_dash.insert(M_dash.end(), mHash.begin(), mHash.end());
	M_dash.insert(M_dash.end(), dbMask.end() - HASH::output_size, dbMask.end());//salt
	mHash = sha.hash(M_dash.begin(), M_dash.end());
	if(equal(mHash.begin(), mHash.end(), em.end() - HASH::output_size)) return true;
	return false;
}

//template<class HASH, int SLen>
//bool PSS<HASH, SLen>::verify(mpz_class sign)
//{
//	mpz_class m = rsa_.encode(sign);
//	uint8_t em[emLen_], p[k_];
//	mpz2bnd(m, em, em + emLen_);
//	mpz2bnd(sign, p, p + k_);
//	return emsa_pss_verify(p, k_, em, emLen_);
//}

//template<class HASH, int SLen>
//PSS<HASH, SLen>::PSS(RSA &r) : rsa_{r}
//{
//	modbit_ = mpz_sizeinbase(rsa_.K.get_mpz_t(), 2);
//	k_ = mpz_sizeinbase(rsa_.K.get_mpz_t(), 256);
//	emLen_ = (modbit_ - 1) % 8 == 0 ? k_ - 1 : k_;
//}
