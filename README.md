# Working Telemetry

This repo contains the files for sending, receiving, and visualizing data we create from given sensors.

## TelemetryApp_SRT

This is a MATLAB app source code file that runs on computers that have the entire MATLAB editor installed. You'll need the [MATLAB App Designer](https://www.mathworks.com/products/matlab/app-designer.html) to edit this file directly.

### Save as .m File

Once you have this installed, you can also save it to a `.m` file. This save allows people without App Designer to view and edit the underlying source code, though it's difficult to pack it back in. Follow these instructions to make your `.m` file:

- In your App Designer program, navigate to the **CANVAS** tab at the top of your screen.
- Under the **File** group, click on the **Save** arrow under the large icon.
- Click **Export to .m File...**.
- Select your location, then click **Save**.

### Export for Use on Other Computers

When you're ready to run the app outside of the App Designer, you'll need to export it to be a `.mlappinstall` file. To do this, follow these steps:

- Go the the MATLAB App Designer.
- At the top of the window, select the **APP** tab.
- Click on **Compiler**. Select **Package App**.
- Fill out the provided boxes as you want. They aren't required, though!
- Click **Package** to create the `.mlappinstall` file.
- Send the compiled file to whoever is going to run it.

#### Run on Other Computers

Other computers won't be able to run the `.mlappinstall` file unless they have the MATLAB Runtime. They can install it from the [MATLAB website](https://www.mathworks.com/products/compiler/matlab-runtime.html).

On Windows:

- Click the `.mlappinstall` file and hope it works...

On Linux:

- Unzip the `.mlappinstall` file
  - In the terminal, you can use `unzip APP_NAME.mlappinstall` to do this.
- Navigate to the folder it makes...
  - `cd APP_NAME`
- Open a terminal, then type `./run_APP_NAME.sh`

Please replace APP_NAME with your exported app's name.
