import 'dart:convert';
import 'dart:io';

class HttpJsonResponse {
  final HttpClientResponse raw;
  HttpJsonResponse(this.raw);

  Future<String> asString() {
    return raw.transform(utf8.decoder).join();
  }

  Future<Map<dynamic, dynamic>> asMap() async {
    final data = jsonDecode(await asString());
    if (data is Map) {
      return data;
    } else {
      throw Exception("JSON data is not of Map type (got ${data.runtimeType})");
    }
  }

  Future<List<dynamic>> asList() async {
    final data = jsonDecode(await asString());
    if (data is List) {
      return data;
    } else {
      throw Exception(
          "JSON data is not of List type (got ${data.runtimeType})");
    }
  }
}

class HttpJsonApi {
  static final client = HttpClient();
  final String server;
  final int port;
  final String path;

  HttpJsonApi({required this.server, required this.port, required this.path}) {
    client.connectionTimeout = const Duration(seconds: 10);
  }

  Future<HttpJsonResponse> get() async {
    HttpClientRequest req;
    try {
      req = await client.get(server, port, path);
    } catch (e) {
      rethrow;
    }
    req.headers.add(HttpHeaders.acceptHeader, "application/json");
    return HttpJsonResponse(await req.close());
  }

  Future<HttpJsonResponse> post(dynamic data) async {
    final strData = json.encode(data);
    print("Post data: $strData");
    HttpClientRequest req;
    try {
      req = await client.post(server, port, path);
    } catch (e) {
      rethrow;
    }
    req.headers.chunkedTransferEncoding = false;
    req.headers.contentType = ContentType.json;
    req.headers.contentLength = strData.length;
    req.bufferOutput = false;
    req.write(strData);
    // await req.flush();
    // req.
    // await req.done;
    final resp = await req.close();
    print("Status code: ${resp.statusCode}");
    return HttpJsonResponse(resp);
  }
}
