
Enabling support of probes in µVision IDE

For some platforms (like Windows 10) using probes in the µVision IDE requires a custom debug sequence. 
Without this support, debugging with these probes may fail.

You add support for these probes by installing the cypress.uvision_support.1.0.0.pack. The pack is located here:
<PDL 3.x install folder>tools\probes\cypress.uvision_support.1.0.0.pack

To install the pack, locate the file and double-click. The Pack Installer installs the necessary files in the µVision IDE.
This is a one-time step that installs support in the IDE. You do not need to repeat this for every project. If you reinstall the µVision IDE, you may need to reinstall this software pack.
A PDL template project uses a generic device. After installing the pack, select the device. Use either CY8C6:CortexM0p or CY8C6:CortexM4 as appropriate.  

To use a debug probe, do the following.
1. In the Options for Target window, click the Debug tab. Select the appropriate probe and click Settings. On the Debug tab:
   For ULink2/ULink Pro debuggers, do the following:
   - Select "VECTRESET" for Reset.
   - Set AP (Access Port) settings: For CM0p, use 0x01; for CM4, use 0x02
   For Segger J-Link Debuggers, select "Core" for Reset. Select "Core" when debugging the CM0p and "Normal" when debugging the CM4. 
2. In the Settings window, click the Pack tab, and check that pack support is enabled.
3. Click the Flash Download tab, and check that flash loaders are added.
4. Check if RAM for Algorithm is correct:
   - For Cortex CM0+ core project: Start: 0x08002400and Size: 0x8000
   - For Cortex CM4 core project: Start: 0x08026400 and Size: 0x8000 
5. Click OK to exit the Settings and the Target for Options windows, saving your settings.

You can now program the target and debug the project.
NOTE In some cases, after flashing you will need to reset the device to run the programmed application.

For more details see the Peripheral Driver Library User Guide 