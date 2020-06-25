#include<fstream>
#include<sha256.h>
#include"adnet.h"
using namespace std;

void Adnet::process()
{
	if(requested_document_ == "signup.php") content_ = signup();
	else if(requested_document_ == "signup.html") ;
	else if(requested_document_ == "index.html") index();//from login button
	else if(requested_document_ == "banner.html") banner();
	else id_hit();//adnet.zeta2374.com/techlead
}

void Adnet::banner()
{
	for(int i=0; i<3; i++) swap("@ID", id_);
	if(nameNvalue_["leaderfilename"] != "") {
		ofstream f{"banner/" + id_ + "-leader.jpg"};
		f << nameNvalue_["leader"];
	}
	if(nameNvalue_["mobilefilename"] != "") {
		ofstream f{"banner/" + id_ + "-mobile.jpg"};
		f << nameNvalue_["mobile"];
	}
	if(nameNvalue_["squarefilename"] != "") {
		ofstream f{"banner/" + id_ + "-square.jpg"};
		f << nameNvalue_["square"];
	}
	if(string s = nameNvalue_["link"]; s != "")
		sq.query("update Users set link = '" + s + "' where id = '" + id_ + "'");
}

string Adnet::signup()
{
	if(nameNvalue_["psw"] != nameNvalue_["psw-repeat"]) return "password not match";
	if(!sq.select("Users", "where email = '" + nameNvalue_["email"] + "'")) 
		return "email already exist";
	if(!sq.select("Users", "where id = '" + nameNvalue_["id"] + "'"))
		return "id already exist";
	if(nameNvalue_["remember"] == "on") id_ = nameNvalue_["id"];
	SHA2 sha;
	auto a = sha.hash(nameNvalue_["psw"].cbegin(), nameNvalue_["psw"].cend());
	string enc = base64_encode({a.begin(), a.end()});
	sq.insert(nameNvalue_["email"], enc, 0, nameNvalue_["id"], 0, 0, 0, 0, "");
	return "you are registered";
}

void Adnet::index()
{
	sq.connect("192.168.0.3", "adnet", "adnetadnet", "adnet");
	if(string s = nameNvalue_["id"]; s != "") {//login
		SHA2 sha;
		string pass = nameNvalue_["psw"];
		auto a = sha.hash(pass.cbegin(), pass.cend());
		pass = base64_encode({a.begin(), a.end()});

		if(sq.select("Users", "where id = '" + s + "' and password = '" + pass + "'"))
			id_ = sq[0]["id"].asString();
	}
	swap("@ID", id_);
	swap("@LOGGED", id_ == "" ? "false" : "true");
}

void Adnet::id_hit()
{
	sq.query("update Users set click_induce = click_induce + 1 where id = '"
			+ requested_document_ + "'");
	content_ = fileNhtml_["index.html"];
	index();
}
