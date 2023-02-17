import 'package:flutter/material.dart';
import 'dart:io';
import 'dart:convert';
import 'package:audioplayers/audioplayers.dart';
import 'package:flutter/services.dart';

void main() {
  runApp(const MyApp());
}

class MyApp extends StatelessWidget {
  const MyApp({super.key});

  // This widget is the root of your application.
  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'playground',
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
      home: const MyHomePage(title: 'udp receive'),
    );
  }
}

class MyHomePage extends StatefulWidget {
  const MyHomePage({super.key, required this.title});

  // This widget is the home page of your application. It is stateful, meaning
  // that it has a State object (defined below) that contains fields that affect
  // how it looks.

  // This class is the configuration for the state. It holds the values (in this
  // case the title) provided by the parent (in this case the App widget) and
  // used by the build method of the State. Fields in a Widget subclass are
  // always marked "final".

  final String title;

  @override
  State<MyHomePage> createState() => _MyHomePageState();
}

class DigitalInputsControl {
  var DI = [2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2];
  void updateDI(int index, int value) {
    DI[index] = value;
  }

  bool changedToZero(int index, int value) {
    if (DI[index] == 1 && value == 0) {
      return true;
    } else {
      return false;
    }
  }

  int summary() {
    var sum = 0;
    for (var i = 0; i < 16; i++) {
      sum += DI[i];
    }
    return sum;
  }
}

class _MyHomePageState extends State<MyHomePage> {
  int _counter = 0;
  static const int UDP_BROADCAST_SELF_PORT = 65500;
  String message = "no message";
  var stateDI = DigitalInputsControl();
  AudioPlayer player = AudioPlayer();
  Uint8List soundbytes = Uint8List(0);
  void _updateState() {
    setState(() {
      // This call to setState tells the Flutter framework that something has
      // changed in this State, which causes it to rerun the build method below
      // so that the display can reflect the updated values. If we changed
      // _counter without calling setState(), then the build method would not be
      // called again, and so nothing would appear to happen.
      _counter++;
    });
  }

  playSound_auch_daniska() async {
    await player.play(AssetSource("sounds/auch_daniska.mp3"));
  }

  playSound_hehehe_mama() async {
    await player.play(AssetSource("sounds/hehehe_mama.mp3"));
  }

  playSound_hou_ou() async {
    await player.play(AssetSource("sounds/hou_ou.mp3"));
  }

  playSound_laghing_kids() async {
    await player.play(AssetSource("sounds/laghing_kids.mp3"));
  }

  playSound_ups_daniska() async {
    await player.play(AssetSource("sounds/ups_daniska.mp3"));
  }

  /*constant udp listening */
  _MyHomePageState() {
    _counter = 0;
    RawDatagramSocket.bind(InternetAddress.anyIPv4, UDP_BROADCAST_SELF_PORT)
        .then((RawDatagramSocket socket) {
      print('Datagram socket ready to receive');
      print('${socket.address.address}:${socket.port}');
      socket.listen((RawSocketEvent e) {
        Datagram? d = socket.receive();
        if (d != null) {
          var decodeSucceeded = false;
          Map<String, dynamic> decodedJSON = {};
          try {
            decodedJSON = json.decode(String.fromCharCodes(d.data))
                as Map<String, dynamic>;
            decodeSucceeded = true;
          } on FormatException catch (e) {
            print('The provided string is not valid JSON');
          }
          if (decodeSucceeded) {
            final name = decodedJSON['name'] as String?;
            if (name != null) {
              if (name == "digital_input_states") {
                message = name;
                for (var i = 0; i < 16; i++) {
                  if (stateDI.changedToZero(i, decodedJSON['di_state_${i}'])) {
                    print("DI ${i} changed to 0 lets play sound");
                    playSound_auch_daniska();
                  }
                  stateDI.updateDI(i, decodedJSON['di_state_${i}']);
                }
                print(
                    'Datagram ${d.address.address}:${d.port}: ${decodedJSON}');
                _updateState();
              }
            }
          }
        }
      });
    });
  }
  @override
  Widget build(BuildContext context) {
    // This method is rerun every time setState is called, for instance as done
    // by the _incrementCounter method above.
    //
    // The Flutter framework has been optimized to make rerunning build methods
    // fast, so that you can just rebuild anything that needs updating rather
    // than having to individually change instances of widgets.
    return Scaffold(
      appBar: AppBar(
        // Here we take the value from the MyHomePage object that was created by
        // the App.build method, and use it to set our appbar title.
        title: Text(widget.title),
      ),
      body: Center(
        // Center is a layout widget. It takes a single child and positions it
        // in the middle of the parent.
        child: Column(
          // Column is also a layout widget. It takes a list of children and
          // arranges them vertically. By default, it sizes itself to fit its
          // children horizontally, and tries to be as tall as its parent.
          //
          // Invoke "debug painting" (press "p" in the console, choose the
          // "Toggle Debug Paint" action from the Flutter Inspector in Android
          // Studio, or the "Toggle Debug Paint" command in Visual Studio Code)
          // to see the wireframe for each widget.
          //
          // Column has various properties to control how it sizes itself and
          // how it positions its children. Here we use mainAxisAlignment to
          // center the children vertically; the main axis here is the vertical
          // axis because Columns are vertical (the cross axis would be
          // horizontal).
          mainAxisAlignment: MainAxisAlignment.center,
          children: <Widget>[
            const Text(
              'udp message',
            ),
            Text(
              message,
              style: Theme.of(context).textTheme.headlineMedium,
            ),
          ],
        ),
      ),
      floatingActionButton: FloatingActionButton(
        onPressed: _updateState,
        child: const Icon(Icons.add),
      ), // This trailing comma makes auto-formatting nicer for build methods.
    );
  }
}
