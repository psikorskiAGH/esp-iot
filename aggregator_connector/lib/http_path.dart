import 'dart:async';
import 'dart:convert';

import 'package:flutter/material.dart';
import 'package:web_socket_channel/web_socket_channel.dart';
import 'package:http/http.dart';

class HttpJsonResponse {
  final dynamic jsonValue;
  HttpJsonResponse(this.jsonValue) {
    if (jsonValue is Map &&
        jsonValue["status"] is int &&
        (jsonValue["status"] / 200).floor() != 200) {
      throw Exception(
          "Got response with status ${jsonValue["status"]}: $jsonValue");
    }
  }

  // static Future<HttpJsonResponse> fromHttpClientResponse(
  //     HttpClientResponse raw) async {
  //   final str = await raw.transform(utf8.decoder).join();
  //   return HttpJsonResponse.fromString(str);
  // }

  static HttpJsonResponse fromString(String data) {
    return HttpJsonResponse(jsonDecode(data));
  }

  Map<dynamic, dynamic> asMap() {
    if (jsonValue is Map) {
      return jsonValue;
    } else {
      throw Exception(
        "JSON data is not of Map type (got ${jsonValue.runtimeType}): $jsonValue",
      );
    }
  }

  List<dynamic> asList() {
    if (jsonValue is List) {
      return jsonValue;
    } else {
      throw Exception(
        "JSON data is not of List type (got ${jsonValue.runtimeType}): $jsonValue",
      );
    }
  }
}

class JsonApiHandlers {
  /* === General === */
  static const _timeoutDuration = Duration(seconds: 10);

  final String _server;
  final int _port;
  final String _path;

  JsonApiHandlers({
    required String server,
    required int port,
    String path = '',
  })  : _path = path,
        _port = port,
        _server = server {
    /* _wssAwaitingRequestsTimer = */
    Timer.periodic(const Duration(seconds: 1), (timer) {
      _wssAwaitingRequestTimeouts();
    });
  }

  /* === WebSocket === */

  WebSocketChannel? _wssClient;
  final Map<int, (Completer<HttpJsonResponse>, DateTime)> _wssAwaitingRequests =
      {}; // DateTime = start time + timeout
  int _wssRequestPointer = 0;
  // late final Timer _wssAwaitingRequests Timer;

  void _wssAwaitingRequestTimeouts() {
    final toDel = <int>[];
    for (var el in _wssAwaitingRequests.entries) {
      if (el.value.$2.compareTo(DateTime.now()) < 0) {
        el.value.$1
            .completeError(TimeoutException("WebSocket message timeout."));
        toDel.add(el.key);
      }
    }
    for (var id in toDel) {
      _wssAwaitingRequests.remove(id);
    }
  }

  // void _wssClearAwaitingRequests() {
  //   _wssAwaitingRequestsTimer?.cancel();
  //   _wssAwaitingRequestsTimer = null;
  //   for (var el in _wssAwaitingRequests.values) {
  //     el.$1.completeError(const WebSocketException("WebSocket was closed"));
  //   }
  //   _wssAwaitingRequests.clear();
  // }

  void _wssStreamHandler(dynamic rawData) {
    final dynamic data;
    try {
      data = jsonDecode(rawData);
    } catch (e) {
      rethrow;
    }
    if (data is! Map<String, dynamic>) {
      throw Exception(
          "WSS: JSON response is not of Map type (got ${data.runtimeType})");
    }
    // Here you can extract response info, eg. status code

    final statusCode = data["status"];
    if (statusCode is! int) {
      throw Exception("WSS: Got response without correct status code ($data).");
    }
    if ((statusCode / 100).floor() != 2) {
      throw Exception(
          "WSS: Got response with status code $statusCode ($data).");
    }

    final id = data["id"];
    if (id is! int || id < 0) {
      throw Exception("WSS: Got response without correct ID ($data).");
    }

    final waitingReq = _wssAwaitingRequests[id];
    if (waitingReq == null) {
      throw Exception(
          "WSS: Request waiting for response with ID $id does not exist.");
    }
    waitingReq.$1.complete(HttpJsonResponse(data["body"]));
    _wssAwaitingRequests.remove(id);
  }

  Future<HttpJsonResponse> _wssProcessRequest(Map<String, dynamic> data) async {
    final id = _wssRequestPointer++;
    data["id"] = id;
    final rawData = jsonEncode(data);
    final completer = Completer<HttpJsonResponse>();
    _wssAwaitingRequests[id] =
        (completer, DateTime.now().add(_timeoutDuration));
    _wssClient!.sink.add(rawData);

    return completer.future;
  }

  Future<HttpJsonResponse> _wssGet({String subpath = ''}) async {
    return _wssProcessRequest({
      "uri": _path + subpath,
      "method": "GET",
    });
  }

  Future<HttpJsonResponse> _wssPost(dynamic body, {String subpath = ''}) async {
    return _wssProcessRequest({
      "uri": _path + subpath,
      "method": "POST",
      "body": body,
    });
  }

  /* === HTTP === */

  static final _clientHttp = Client();

  Future<HttpJsonResponse> _httpGet({String subpath = ''}) async {
    Response resp;
    try {
      resp = await _clientHttp.get(
        Uri(scheme: 'http', host: _server, port: _port, path: _path + subpath),
        headers: {"Accept": "application/json"},
      );
    } catch (e) {
      rethrow;
    }
    resp.headers["accept"] = "application/json";
    return HttpJsonResponse.fromString(resp.body);
  }

  Future<HttpJsonResponse> _httpPost(dynamic data,
      {String subpath = ''}) async {
    final strData = json.encode(data);
    Response resp;
    try {
      resp = await _clientHttp.post(
        Uri(scheme: 'http', host: _server, port: _port, path: _path + subpath),
        body: strData,
        headers: {"Content-type": "application/json"},
      );
    } catch (e) {
      rethrow;
    }
    // req.headers.chunkedTransferEncoding = false;
    // req.headers.contentType = ContentType.json;
    // req.headers.contentLength = strData.length;
    // req.bufferOutput = false;
    // req.write(strData);

    return HttpJsonResponse.fromString(resp.body);
  }

  /* === User facing methods === */

  Future<void> useWebSocket({String wssSubpath = '/websocket'}) {
    final WebSocketChannel channel;
    try {
      channel = WebSocketChannel.connect(Uri.parse("ws://$_server$wssSubpath"));
    } catch (e) {
      // In case of failure, retry
      return useWebSocket(wssSubpath: wssSubpath);
    }

    // In case of failure, retry
    channel.sink.done.then((value) {
      if (_wssClient == channel) {
        useWebSocket(wssSubpath: wssSubpath);
      }
    });

    return channel.ready.then((value) {
      // Awaits readinness before switching to that socket
      channel.stream.listen(_wssStreamHandler);
      _wssClient = channel;
    });
  }

  void useHttp() {
    _wssClient = null;
  }

  bool isUsingWebSocket() => _wssClient != null;

  Future<HttpJsonResponse> get({String subpath = ''}) {
    if (_wssClient == null) {
      return _httpGet(subpath: subpath);
    } else {
      return _wssGet(subpath: subpath);
    }
  }

  Future<HttpJsonResponse> post(dynamic data, {String subpath = ''}) {
    if (_wssClient == null) {
      return _httpPost(data, subpath: subpath);
    } else {
      return _wssPost(data, subpath: subpath);
    }
  }
}

class JsonApi {
  final JsonApiHandlers handlers;
  late final String subpath;

  JsonApi({required this.handlers, String? subpath}) {
    if (subpath == null) {
      this.subpath = '';
    } else {
      if (handlers._path.characters.lastOrNull != '/' &&
          subpath.characters.firstOrNull != '/') {
        subpath = '/$subpath';
      }
      this.subpath = subpath;
    }
  }

  Future<HttpJsonResponse> get() {
    return handlers.get(subpath: subpath);
  }

  Future<HttpJsonResponse> post(dynamic data) {
    return handlers.post(data, subpath: subpath);
  }
}
