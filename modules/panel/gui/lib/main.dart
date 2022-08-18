import 'package:flutter/material.dart';
import 'dart:developer';

void main() {
  runApp(const MyApp());
}

class MyApp extends StatelessWidget {
  const MyApp({super.key});

  // This widget is the root of your application.
  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'playos-panel',
      debugShowCheckedModeBanner: false,
      theme: ThemeData(
        // This is the theme of your application.
        //
        // Try running your application with "flutter run". You'll see the
        // application has a blue toolbar. Then, without quitting the app, try
        // changing the primarySwatch below to Colors.green and then invoke
        // "hot reload" (press "r" in the console where you ran "flutter run",
        // or simply save your changes to "hot reload" in a Flutter IDE).
        // Notice that the counter didn't reset back to zero; the application
        // is not restarted.
        primarySwatch: Colors.blue,
      ),
      home: const PanelPage(),
    );
  }
}

class PanelPage extends StatefulWidget {
  const PanelPage({super.key});

  @override
  State<PanelPage> createState() => _PanelPageState();
}

class _PanelPageState extends State<PanelPage> {
  String time = "20:20:55";
  String battery = "100%";

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      backgroundColor: const Color.fromRGBO(255, 255, 255, 0.9),
      body: Center(
        child: Padding(
          padding: const EdgeInsets.symmetric(horizontal: 8.0),
          child: Row(
            mainAxisAlignment: MainAxisAlignment.center,
            crossAxisAlignment: CrossAxisAlignment.center,
            children: [
              Padding(
                padding: const EdgeInsets.only(left: 4),
                child: Text(time),
              ),
              const Spacer(),
              const Icon(
                Icons.wifi,
              ),
              Text(battery),
              const Icon(
                Icons.battery_charging_full_sharp,
              ),
            ],
          ),
        ),
      )
    );
  }
}
