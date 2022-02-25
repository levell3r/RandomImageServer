import os
import sys
import subprocess
from flask import Flask, Response
from subprocess import Popen

#create Flask App
app = Flask(__name__)
i = 0

# Setup Flask API App
@app.route("/load")
def load():
    #cmd = "nohup python3 /home/dan/miniwebserver/imageserver1.py &"
    subprocess.run("nohup python3 /home/dan/miniwebserver/imageserver1.py &", shell=True, check=True)
    return 'loading ImageServer'

@app.route("/shutdown")
def shutdown():
    #cmd = "pkill -9 -f imageserver1.py"
    subprocess.run("pkill -9 -f imageserver1.py", shell=True, check=True)
 
    return 'shutdown KILL server'

app.run(host='0.0.0.0', port=54321),