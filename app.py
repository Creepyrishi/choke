from flask import Flask, jsonify, request, render_template
import serial

app = Flask(__name__)

# Set your Arduino port and baud rate
ARDUINO_PORT = "COM3"  # Change this for your system (e.g., "/dev/ttyUSB0" for Linux/Mac)
BAUD_RATE = 9600

# Open serial connection
try:
    ser = serial.Serial(ARDUINO_PORT, BAUD_RATE, timeout=1)
except Exception as e:
    print(f"Error: {e}")
    ser = None

@app.route("/")
def home():
    return render_template("index.html")

@app.route("/cattle.html")
def cattle():
    return render_template("cattle.html")

@app.route("/farm.html")
def farm():
    return render_template("farm.html")

@app.route("/frontend.html")
def frontend():
    return render_template("frontend.html")

@app.route("/sensor-data", methods=["GET"])
def get_sensor_data():
    """Fetch sensor data from Arduino via Serial and return as JSON."""
    if ser and ser.in_waiting > 0:
        line = ser.readline().decode('utf-8').strip()
        if line:
            try:
                # Parse the received data
                parts = line.split(", ")
                temp = float(parts[0].split(": ")[1][:-1])  # Remove 'C' and convert
                humidity = float(parts[1].split(": ")[1][:-1])  # Remove '%' and convert
                nutrition = int(parts[2].split(": ")[1])  # Convert to int

                return jsonify({
                    "temperature": temp,
                    "humidityLevel": humidity,
                    "nutrientLevel": nutrition,
                    "heart-rate": 0
                })
            except Exception as e:
                return jsonify({"error": f"Failed to parse data: {e}"})
    return jsonify({"error": "No data received from Arduino"})


@app.route("/send-data", methods=["POST"])
def send_data():
    """Send data to Arduino via Serial using POST parameters."""
    if ser:
        try:
            # Extract data from JSON body
            data = request.get_json()
            waterPump = data.get("waterPump")
            stirGobar = data.get("stirGobar")
            conveyGobar = data.get("nutrition")

            # Check if data is provided in the request
            if waterPump is not None and stirGobar is not None and conveyGobar is not None:
                # Format the message to send to Arduino
                message = f"[{waterPump}, {stirGobar}, {conveyGobar}]\n"

                print(message)
                # Send the data to Arduino via serial
                ser.write(message.encode('utf-8'))

                return jsonify({"status": "Data sent to Arduino"})
            else:
                return jsonify({"error": "Missing parameters in the request. Please include 'waterPump', 'stirGobar', and 'nutrition'."})
        except Exception as e:
            return jsonify({"error": f"Failed to send data to Arduino: {e}"})
    return jsonify({"error": "Serial connection not available"})


if __name__ == "__main__":
    app.run(debug=True, host="0.0.0.0", port=5000)
