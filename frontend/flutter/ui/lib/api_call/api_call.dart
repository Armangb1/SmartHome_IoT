import 'package:flutter/material.dart';
import 'package:http/http.dart' as http;
import 'dart:convert';
import 'login_token.dart' as Token;
import 'api_constants.dart';

abstract class ClientApi {
  static Future<bool> login(String username, String password) async {
    bool isLogedIn = false;

    var url =
        Uri(host: host, port: port, path: "api/auth/token/", scheme: scheme);

    var response = await http
        .post(url, body: {"username": username, "password": password});

    var data = jsonDecode(response.body);
    Token.token = data['access'];
    if (response.statusCode == 200) {
      isLogedIn = true;
    }
    return isLogedIn;
  }

  static Future<bool> writeActuator(String path, bool state) async {
    bool isSent = false;
    Uri uri =
        Uri(scheme: scheme, host: host, port: port, path: "api/write/$path/");

    int value = state ? 1 : 0;

    String? token = Token.token;

    var respone = await http.post(uri,
        body: {"value": value.toString()},
        headers: {"authorization": "Bearer $token"});

    if (respone.statusCode == 200) {
      isSent = true;
      debugPrint(respone.body);
    }

    return isSent;
  }
}
