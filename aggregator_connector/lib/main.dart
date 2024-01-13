import 'package:aggregator_connector/device/server.dart';
import 'package:flutter/material.dart';
import 'package:aggregator_connector/http_path.dart';
import 'package:flutter/foundation.dart';

import 'package:window_manager/window_manager.dart';

// ignore: constant_identifier_names
const String APP_TITLE = "Device Viewer";

void main() async {
  // Source: https://www.togaware.com/linux/survivor/flutter-desktop-app-window-manager.html#flutter-desktop-app-window-manager

  print(defaultTargetPlatform);
  bool isDesktop = [TargetPlatform.linux, TargetPlatform.macOS, TargetPlatform.windows].contains(defaultTargetPlatform);

  if (isDesktop && !kIsWeb) {

    WidgetsFlutterBinding.ensureInitialized();

    await windowManager.ensureInitialized();

    WindowOptions windowOptions = const WindowOptions(
      alwaysOnTop: true,
      // size: Size(450, 700),
      title: APP_TITLE,
      center: true,
      // backgroundColor: Colors.transparent,
      // skipTaskbar: false,
      // titleBarStyle: TitleBarStyle.hidden,
    );

    windowManager.waitUntilReadyToShow(windowOptions, () async {
      await windowManager.show();
      await windowManager.focus();
      await windowManager.setAlwaysOnTop(false);
    });
  }
  runApp(const MainApp());
}

class MainApp extends StatefulWidget {
  const MainApp({super.key});

  static Map<String, JsonApiHandlers> createdApiHandlers =
      {}; // server: widgetKey

  static JsonApiHandlers getHandlers(
      String serverIP, int serverPort, String serverRootPath) {
    JsonApiHandlers? handler = createdApiHandlers[serverIP];
    if (handler == null) {
      handler = JsonApiHandlers(
          server: serverIP, port: serverPort, path: serverRootPath);
      createdApiHandlers[serverIP] = handler;
    }
    return handler;
  }

  @override
  State<StatefulWidget> createState() {
    return _MainApp();
  }
}

class _MainApp extends State<MainApp> {
  String serverIP = '192.168.1.107';
  int serverPort = 80;
  String serverRootPath = '';
  int deviceServerKey = 0;

  _MainApp() {
    // apiHandlers = JsonApiHandlers(server: serverIP, port: serverPort, path: serverRootPath);
  }

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      home: Scaffold(
        body: Builder(builder: (context) {
          final key = Key((++deviceServerKey).toString());
          final hadlers =
              MainApp.getHandlers(serverIP, serverPort, serverRootPath);
          return DeviceServer(
            key: key,
            apiHandlers: hadlers,
          );
        }),
        appBar: AppBar(
          title: _getForm(),
        ),
      ),
      title: APP_TITLE,
    );
  }

  Form _getForm() {
    return Form(
      child: Row(
        children: [
          Expanded(
            child: TextFormField(
              autocorrect: false,
              decoration: const InputDecoration(
                labelText: "Server IP",
              ),
              initialValue: serverIP,
              onChanged: (value) {
                serverIP = value;
              },
            ),
          ),
          SizedBox(
            width: 100,
            child: ElevatedButton(
              onPressed: () {
                setState(() {});
              },
              child: const Text("Reload"),
            ),
          )
        ],
      ),
    );
  }
}
