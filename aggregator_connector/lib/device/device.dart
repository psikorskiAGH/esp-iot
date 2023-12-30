import 'dart:io';

import 'package:aggregator_connector/device/info.dart';
import 'package:aggregator_connector/http_path.dart';
import 'package:flutter/material.dart';

import 'config.dart';
import 'data.dart';

String oscIP = '192.168.1.107';
var client = HttpClient();

class Device extends StatelessWidget {
  final int refreshPeriod;

  const Device({super.key, this.refreshPeriod = 100});

  @override
  Widget build(BuildContext context) {
    return Column(
      // scrollDirection: Axis.vertical,
      // shrinkWrap: true,
      children: [
        DeviceInfo(
          ctx: DeviceInfoContext(
            api: HttpJsonApi(server: oscIP, port: 80, path: "/oscilloscope"),
          ),
        ),
        Row(
          children: <Widget>[
            // Expanded(child: Center(child: oscChart)),
            // const Expanded(child: Center(child: Text("Hi!")))
            Expanded(
              child: DeviceData(
                ctx: DeviceDataContext(
                  api: HttpJsonApi(
                      server: oscIP, port: 80, path: "/oscilloscope/data"),
                ),
              ),
            ),
            Expanded(
              child: DeviceConfig(
                ctx: DeviceConfigContext(
                  api: HttpJsonApi(
                      server: oscIP, port: 80, path: "/oscilloscope/config"),
                ),
              ),
            ),
          ],
        ),
      ],
    );
  }
}
