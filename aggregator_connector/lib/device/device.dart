import 'package:aggregator_connector/device/info.dart';
import 'package:aggregator_connector/http_path.dart';
import 'package:flutter/material.dart';

import 'config.dart';
import 'data.dart';

String oscIP = '192.168.1.107';
// var client = HttpClient();
final apiHandlers =
    JsonApiHandlers(server: oscIP, port: 80, path: "/devices/oscilloscope");

class Device extends StatelessWidget {
  Device({super.key}) {
    // apiHandlers.useWebSocket();
  }

  @override
  Widget build(BuildContext context) {
    return Column(
      // scrollDirection: Axis.vertical,
      // shrinkWrap: true,
      children: [
        DeviceInfo(
          ctx: DeviceInfoContext(
            api: JsonApi(handlers: apiHandlers, subpath: ""),
          ),
        ),
        Row(
          children: <Widget>[
            // Expanded(child: Center(child: oscChart)),
            // const Expanded(child: Center(child: Text("Hi!")))
            Expanded(
              child: DeviceData(
                ctx: DeviceDataContext(
                  api: JsonApi(handlers: apiHandlers, subpath: "data"),
                ),
              ),
            ),
            Expanded(
              child: DeviceConfig(
                ctx: DeviceConfigContext(
                  api: JsonApi(handlers: apiHandlers, subpath: "config"),
                ),
              ),
            ),
          ],
        ),
      ],
    );
  }
}
