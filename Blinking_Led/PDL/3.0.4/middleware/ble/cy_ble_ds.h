/***************************************************************************//**
* \file cy_ble_ds.h
* \version 1.0
*
* \brief
*  Contains the documentation data.
*
********************************************************************************
* \copyright
* Copyright 2016-2018, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/
/** 
* \mainpage Cypress BLE Middleware Library
*

\copydoc page_ble_general
\copydoc page_ble_functional_description
\copydoc page_group_ble_changelog


**
**\page page_ble_general General Description
**

The Bluetooth Low Energy (BLE) middleware contains a comprehensive API list to allow you to configure the BLE Stack and 
the underlying chip hardware.

 <b>Features:</b>
  - Multi-link support up to four simultaneous connections in any combination of roles
  - Bluetooth v5.0 compliant protocol stack
      - LE 2 Mbps
  - Generic Access Profile (GAP) Features
      - Broadcaster, Observer, Peripheral and Central roles
      - User-defined advertising data
      - Bonding support for up to 128 devices
      - Security modes 1 and 2
  - Generic Attribute Profile (GATT) Features
      - GATT Client and Server
      - 16-, 32-, and 128-bit UUIDs
  - Special Interest Group (SIG) adopted GATT-based Profiles and Services, and quick prototype of new profile design
    through intuitive GUI Custom Profile development; Support of Bluetooth Developer Studio Profile format
  - Security Manager features
      - Pairing methods: Just works, Passkey Entry, Out of Band, Numeric Comparison
      - Authenticated man-in-the-middle (MITM) protection and data signing
  - Logical Link Adaption Protocol (L2CAP) Connection Oriented Channel
  - Link Layer (LL) Features
      - Master and Slave role
      - 128-bit AES encryption
      - Low Duty Cycle Advertising
      - LE Ping

The BLE middleware incorporates a Bluetooth Core Specification v5.0 compliant protocol stack.
You may access the GAP, GATT and L2CAP layers of the stack using provided API.

The API are broadly categorized as follows:
 - \ref group_ble_common_api
 - \ref group_ble_service_api
 - \ref group_ble_clock_api_section      
      
*      
*\section group_ble_when_use When to use the BLE Middleware
*
BLE is used in very low-power network and Internet of Things (IoT) solutions aimed for low-cost battery operated devices 
that can quickly connect and form simple wireless links. Target applications include HID, remote controls, sports and
fitness monitors, portable medical devices and smart phone accessories, among many others that are being added to a long 
list of BLE supporting solutions.

*
*\section group_ble_sig_adopted SIG adopted Profiles and Services
*
The BLE_PDL Component supports numerous SIG-adopted GATT-based Profiles and Services. Each of these can be configured for
either a GATT Client or GATT Server. The Component generates all the necessary code for a particular Profile/Service 
operation, as configured in the Component Configure dialog.

*
*\section group_ble_custom_profile Custom Profiles
*
You can create custom Profiles that use existing Services, and you can create custom Services with custom Characteristics
and Descriptors. 

*
*\section group_ble_debug_support Debug Support
*
For testing and debugging, the Component can be configured to HCI mode through a Component-embedded UART. 
See General Tab - BLE Controller only (HCI over UART).
For over-the-air verification, the Cypress CySmart Central Emulation Tool can be used for generic Bluetooth host stack 
emulation. To launch this tool, right-click on the Component and select Launch CySmart. 

*
*\section group_ble_architecture BLE Middleware Architecture
* 

The BLE_PDL Component consists of the BLE Stack, BLE Profile, BLE_PDL Component Hardware Abstraction Layer (HAL), and the
Link Layer. The following figure shows a high-level architecture of the BLE_PDL Component, illustrating the relationship
between each of the layers and the route in which the application interacts with the Component. Note that the application
is informed of the BLE events through the use of callback functions. You may build your state machine using these. 
Refer to the Callback Functions section for more details.

\htmlonly <style>div.image img[src="ble_architecture.png"]{width:613px;}</style> \endhtmlonly 
\image html ble_architecture.png
 
The following sub-sections give an overview of each of these layers.

*
*\subsection  group_ble_stack BLE Stack
*
The BLE stack implements the core BLE functionality as defined in Bluetooth Core Specification 5.0. The stack is included 
as a precompiled library and is included in the BLE Middleware library. The BLE Stack implements the layered architecture 
of the BLE protocol stack as shown in the following figure.

\image html ble_stack.png

    \subsubsection subsubsection_ble_stack_1 Generic Access Profile (GAP) 
    The Generic Access Profile defines the generic procedures related to discovery of Bluetooth devices and link 
    management aspects of connecting to Bluetooth devices. In addition, this profile includes common format requirements 
    for parameters accessible on the user interface level.

    The Generic Access Profile defines the following roles when operating over the LE physical channel:
    - <b>Broadcaster role</b>: A device operating in the Broadcaster role can send advertising events. It is referred to 
    as a Broadcaster. It has a transmitter and may have a receiver.
    - <b>Observer role</b>: A device operating in the Observer role is a device that receives advertising events. It is 
    referred to as an Observer. It has a receiver and may have a transmitter.
    - <b>Peripheral role</b>: A device that accepts the establishment of an LE physical link using any of the connection 
    establishment procedures is termed to be in a "Peripheral role." A device operating in the Peripheral role will be
    in the "Slave role" in the Link Layer Connection State. A device operating in the Peripheral role is referred to as 
    a Peripheral. A Peripheral has both a transmitter and a receiver.
    - <b>Central role</b>: A device that supports the Central role initiates the establishment of a physical connection. 
    A device operating in the "Central role" will be in the "Master role" in the Link Layer Connection. A device 
    operating in the Central role is referred to as a Central. A Central has a transmitter and a receiver.

    \subsubsection subsubsection_ble_stack_2 Generic Attribute Profile (GATT)
    The Generic Attribute Profile defines a generic service framework using the ATT protocol layer. This framework 
    defines the procedures and formats of services and their Characteristics. It defines the procedures for Service, 
    Characteristic, and Descriptor discovery, reading, writing, notifying, and indicating Characteristics, as well as
    configuring the broadcast of Characteristics.
    GATT Roles
        - <b>GATT Client</b>: This is the device that wants data. It initiates commands and requests towards the GATT 
        Server. It can receive responses, indications, and notifications data sent by the GATT Server.
        - <b>GATT Server</b>: This is the device that has the data and accepts incoming commands and requests from the
        GATT Client and sends responses, indications, and notifications to a GATT Client. The BLE Stack can support both
        roles simultaneously.

    \subsubsection subsubsection_ble_stack_3 Attribute Protocol (ATT)
    The Attribute Protocol layer defines a Client/Server architecture above the BLE logical transport channel.
    The attribute protocol allows a device referred to as the GATT Server to expose a set of attributes and their 
    associated values to a peer device referred to as the GATT Client. These attributes exposed by the GATT Server can 
    be discovered, read, and written by a GATT Client, and can be indicated and notified by the GATT Server. 
    All the transactions on attributes are atomic.

    \subsubsection subsubsection_ble_stack_4 Security Manager Protocol (SMP)
    Security Manager Protocol defines the procedures and behavior to manage pairing, authentication, and encryption 
    between the devices. These include:
        - Encryption and Authentication
        - Pairing and Bonding
            - Pass Key and Out of band bonding
        - Key Generation for a device identity resolution, data signing and encryption
        - Pairing method selection based on the IO capability of the GAP central and GAP peripheral device

    \subsubsection subsubsection_ble_stack_5 Logical Link Control Adaptation Protocol (L2CAP)   
    L2CAP provides a connectionless data channel. LE L2CAP provides the following features:
        - Channel multiplexing, which manages three fixed channels. Two channels are dedicated for higher protocol 
        layers like ATT, SMP. One channel is used for the LE-L2CAP protocol signalling channel for its own use.
        - Segmentation and reassembly of packets whose size is up to the BLE Controller managed maximum packet size. 
        - Connection-oriented channel over a specific application registered using the PSM (protocol service multiplexer) 
        channel. It implements credit-based flow control between two LE L2CAP entities. This feature can be used for BLE 
        applications that require transferring large chunks of data.


    \subsubsection subsubsection_ble_stack_6 Host Controller Interface (HCI)
    The HCI layer implements a command, event, and data interface to allow link layer access from upper layers such as
    GAP, L2CAP, and SMP. 

    \subsubsection subsubsection_ble_stack_7 Link Layer (LL)
    The HCI layer implements a command, event, and data interface to allow link layer access from upper layers such as 
    GAP, L2CAP, and SMP. 
    The LL protocol manages the physical BLE connections between devices. It supports all LL states such as Advertising, 
    Scanning, Initiating, and Connecting (Master and Slave). It implements all the key link control procedures such as LE 
    Encryption, LE Connection Update, LE Channel Update, and LE Ping. The Link Layer is a hardware-firmware 
    co-implementation, where the key time critical LL functions are implemented in the LL hardware. The LL firmware
    maintains and controls the key LL procedure state machines. It supports all the BLE chip specific low-power modes.
    The BLE Stack is a pre-compiled library in the BLE_PDL Component. The appropriate configuration of the BLE Stack 
    library is linked during a build process based on application. The BLE Stack libraries are ARM Embedded Application 
    Binary Interface(EABI) compliant and they are compiled using ARM compiler version 5.03. 
    The following table shows the mapping between the BLE Stack library to the user-configured Complete BLE Protocol mode 
    or HCI mode. Refer to the <b>General Tab</b> section of 
    <a href="http://www.cypress.com/documentation/component-datasheets/bluetooth-low-energy-blepdl">
    Bluetooth Low Energy (BLE_PDL) Datasheet</a> for selection of stack configuration.

<table class="doxtable">
    <tr>
        <th><b>BLE Middleware Configuration</b></th>
        <th><b>CPU Core </b></th>
        <th><b>BLE Stack Libraries</b></th>
    </tr>  
    
    <tr>
        <td rowspan="2"><b>Complete BLE Protocol.</b><br>Host and Controller on single core with software interface </td>
        <td>Single core (Complete Component on CM4) </td>
        <td>cy_ble_stack_gcc_radio_max_cm4.a <br>
            cy_ble_stack_gcc_soc_cm4.a
        </td>
    </tr>
    <tr>
        <td>Single core (Complete Component on CM0+) </td>
        <td>cy_ble_stack_gcc_radio_max_cm0p.a <br>
            cy_ble_stack_gcc_soc_cm0p.a
        </td>
    </tr>
    
    <tr>
        <td><b>Complete BLE Protocol.</b><br>Host and Controller on dual core with IPC interface </td>
        <td>Dual core (Controller on CM0+, Host and Profile on CM4)</td>
        <td>cy_ble_stack_gcc_radio_max_cm0p.a <br>
            cy_ble_stack_gcc_controller_ipc_cm0p.a <br>
            cy_ble_stack_gcc_host_ipc_cm4.a 
        </td>
    </tr>
    
    <tr>
        <td rowspan="2"><b>HCI mode.</b> </td>
        <td>CM4 </td>
        <td>cy_ble_stack_gcc_radio_max_cm4.a <br>
            cy_ble_stack_gcc_controller_uart_cm4.a
        </td>
    </tr>
    <tr>
        <td>CM0+</td>
        <td>cy_ble_stack_gcc_radio_max_cm0p.a <br>
            cy_ble_stack_gcc_controller_uart_cm0p.a
        </td>
    </tr>
</table>
    
BLE Middleware Configuration mode and CPU Core parameters can be configured in the 
<a href="http://www.cypress.com/documentation/component-datasheets/bluetooth-low-energy-blepdl">
<b>General Tab</b> of BLE Configuration Tool</a>.

There are two sets of libraries: 
- "_gcc_" libraries are built with wchar_t typedef set to 32-bit.
- "_mdk_" libraries are built with wchar_t set to 16-bit.
\note The "_gcc_" libraries are used for the IAR compiler.


*
*\subsection  group_ble_profile_layer Profile Layer
*
In BLE, data is organized into concepts called Profiles, Services, and Characteristics. 
    - A <b>Profile</b> describes how devices connect to each other to find and use Services. It is a definition used by 
    Bluetooth devices to describe the type of application and the general expected behavior of that device. See the 
    to the <b>Component Parameters</b> section of 
    <a href="http://www.cypress.com/documentation/component-datasheets/bluetooth-low-energy-blepdl">
    Bluetooth Low Energy (BLE_PDL) Datasheet</a> for how to configure the BLE Middleware. 
    
    - A <b>Service</b> is a collection of data entities called Characteristics. A Service is used to define a certain 
    function in a Profile. A Service may also define its relationship to other Services. A Service is assigned a 
    Universally Unique Identifier (UUID). This is 16 bits for SIG adopted Services and 128 bits for custom Services. 
    See the <b>GATT Settings Tab</b> section of 
    <a href="http://www.cypress.com/documentation/component-datasheets/bluetooth-low-energy-blepdl">
    Bluetooth Low Energy (BLE_PDL) Datasheet</a> for information about adding Services to a Profile.
    
    - A <b>Characteristic</b> contains a Value and the Descriptor that describes a Characteristic Value. It is an
    attribute type for a specific piece of information within a Service. Like a Service, each Characteristic is 
    designated with a UUID; 16 bits for SIG adopted Characteristics and 128 bits for custom Characteristics. 
    See the <b>GATT Settings Tab</b> section of 
    <a href="http://www.cypress.com/documentation/component-datasheets/bluetooth-low-energy-blepdl">
    Bluetooth Low Energy (BLE_PDL) Datasheet</a> for information about adding Characteristics and Descriptors.
    The following diagram shows the relationship between Profiles, Services, and Characteristics in a sample BLE heart 
    rate monitor application using a Heart Rate Profile. 

\htmlonly <style>div.image img[src="ble_profiles.png"]{width:70%;}</style> \endhtmlonly 
\image html ble_profiles.png

The Heart Rate Profile contains a Heart Rate Service and a Device Information Service. Within the Heart Rate Service, 
there are three Characteristics, each containing different information. The device in the diagram is configured as a 
Sensor role, meaning that in the context of the Heart Rate Profile, the device is a GAP Peripheral and a GATT Server. 
These concepts are explained in the \ref group_ble_stack description.

The Profile layer is generated by PSoC Creator using the parameter configurations specified in the GUI. The Profile 
implements the Profile specific attribute database and APIs required for the application. You can choose to configure the 
standard SIG adopted Profile and generate a design or define a Custom Profile required by an application. The GUI also 
allows import/export of a Profile design in XML format for Profile design reuse. In addition, the Bluetooth Developer 
Studio compliant XML format is available.

*
*\subsection  group_ble_hal Hardware Abstraction Layer (HAL)
*
The HAL implements the interface between the BLE stack and the underlying hardware. This layer is meant for the stack 
only and it is not advisable to modify it.

**
**\page page_ble_functional_description Functional Description
**
*

*\section group_ble_operation_flow Operation Flow
*
A typical application code consists of three separate stages: Initialization, Normal operation, and Low-power operation.

\image html ble_operation_flow.png


After initialization, the BLE Middleware enters normal operation and periodically enters various degrees of low-power 
operation to conserve power. Therefore, initialization should only happen at system power-up, and the BLE Middleware 
should operate between normal mode and low-power mode afterwards.
    *\subsection group_ble_system_init System Initialization 
    The initialization stage happens at system power-up or when waking from system hibernation. This stage sets up the 
    platform and the BLE Middleware parameters. The application code should also start the Component and set up the
    callback functions for the event callbacks that will happen in the other modes of operation. 

    *\subsection group_ble_system_normal_operation System Normal Operation 
    Upon successful initialization of the BLE Middleware or hibernate wakeup sequence, the Component enters normal mode. 
    Normal operation first establishes a BLE connection if it is not already connected. It should then process all pending
    BLE events by checking the stack status. This is accomplished by calling Cy_BLE_ProcessEvents(). When all events have
    been processed, it can transmit any data that need to be communicated and enters low-power operation unless there is 
    another pending event. In such a case, it should execute the normal operation flow again. Processing of BLE events
    should be performed at least once in a BLE connection event period. The BLE connection event is configured by the 
    Central device while establishing a connection. 

    *\subsection group_ble_system_lpm System Low-power Operation 
    When there are no pending interrupts in normal operation, the BLE Middleware should be placed in low-power mode.
    It should first enter Sleep mode. The BLE Middleware can enter either CPU sleep or deep sleep mode depending on the 
    state of the BLE interface hardware. If an event happens at any time in low-power mode, it should re-enter normal 
    operation.
    \note The MCU and BLE Sub-System (BLESS) have separate power modes and are able to go to different power modes 
    independent of each other. The check marks in the following table show the possible combination of power modes
    of MCU and BLESS. 
    \par
    <table class="doxtable"> 
      <tr>
        <th rowspan="2">BLESS<br>Power Modes</th>
        <th colspan="5">MCU Power Modes</th>
      </tr>
      <tr>
        <td>Active</td>
        <td>Sleep</td>
        <td>Deep sleep</td>
        <td>Hibernate</td>
        <td>Stop</td>
      </tr>
      <tr>
        <td>Active (idle/Tx/Rx)</td>
        <td align="center">Y</td>
        <td align="center">Y</td>
        <td></td>
        <td></td>
        <td></td>
      </tr>
      <tr>
        <td>Sleep</td>
        <td align="center">Y</td>
        <td align="center">Y</td>
        <td></td>
        <td></td>
        <td></td>
      </tr>
      <tr>
        <td>Deep sleep (ECO off)</td>
        <td align="center">Y</td>
        <td align="center">Y</td>
        <td align="center">Y</td>
        <td></td>
        <td></td>
      </tr>
      <tr>
        <td>Off</td>
        <td></td>
        <td></td>
        <td></td>
        <td align="center">Y</td>
        <td align="center">Y</td>
      </tr>
    </table>

*    
*\section group_ble_device_bonding Device Bonding
*
The BLE Middleware will store the link key of a connection after pairing with the remote device. If a connection is
lost and re-established, the devices will use the previously stored key for the connection.
The BLE stack will update the bonding data in RAM while the devices are connected. If the bonding data is to be retained
during shutdown, the application can use Cy_BLE_StoreBondingData() API to write the bonding data from RAM to the 
dedicated Flash location, as defined by the BLE Middleware. 
Refer to the CE215121_BLE_HID_Keyboard code example for usage details.

*
*\section group_ble_lfclk_conf LFCLK Configuration for Deep Sleep Mode
*
The LFCLK configuration affects the BLE Middleware's ability to operate in deep sleep mode. If the WCO or PILO are chosen
as source clock, then the BLE Middleware CPU deep sleep mode is available for use. However, if the ILO is chosen,
then the BLE Middleware cannot enter CPU deep sleep.

Refer to the <b>Advanced Tab</b> section of 
<a href="http://www.cypress.com/documentation/component-datasheets/bluetooth-low-energy-blepdl">
Bluetooth Low Energy (BLE_PDL) Datasheet </a> for information on how to enable low-power mode in the BLE Middleware. 

\note The BLE middleware uses the LFCLK only during CPU deep sleep mode, so ILO inaccuracy does not affect BLE communication.

*
*\subsection group_ble_lfclk_Pilo_conf Configuring PILO for Deep Sleep Mode
*
The BLE Middleware supports using the Precision ILO (PILO) (instead of the WCO) as the LF clock source.

Using the PILO has limitations:
    - supports only a single connection
    - supports only the Peripheral/Slave role
	- not supported for preproduction hardware.

Using the PILO as the LF clock source requires:
    - initial trimming and measuring the step size of the PILO trimming before BLE start
    - setting the clock configuration of the BLE sub-system after BLE start
Both actions must happen during every power-up to get closest to the target frequency.

The following code snippet shows the implementation: 
\snippet ble_sut_01_all_api.cydsn/snippets.c LFCLK configuration: PILO calibration

To set the clock configuration of the BLE sub-system use function Cy_BLE_SetBleClockCfgParam(&param) 
after CY_BLE_EVT_STACK_ON event. The param structure must have the following field values for the sleep clock accuracy and Link Layer clock divider:
- param->bleLlSca = CY_BLE_LL_SCA_251_TO_500_PPM, 
- param->bleLlClockDiv = CY_BLE_LL_ECO_CLK_DIV_2.

Note: Trimming functions PILO_InitialTrim and PILO_MeasureStepSize must be called before Cy_BLE_Start(), and must be called
on the CPU core running the BLE controller. 


* 
*\section group_ble_multiconnection_support Multi-Connection Support
*
The BLE Middleware supports up to four simultaneous Multi-Master Multi-Slave (MMMS) BLE connections in any combination of roles.
For example, it can be a master of four slave devices (4M), a master of three slave devices and a slave of another device 
(3M1S), or a slave of four devices (4S), or any other combination.
To configure the maximum number of BLE connections, refer to the <b>General Tab</b> section of 
<a href="http://www.cypress.com/documentation/component-datasheets/bluetooth-low-energy-blepdl">
Bluetooth Low Energy (BLE_PDL) Datasheet</a>(Maximum number of BLE connections).
The BLE Middleware supports a single instance of a GATT Server (single GATT database). The number of the Server instance
field is always fixed to 1 in the BLE_PDL Configure dialog. You can add additional Services or a complete Server Profile 
to the existing Server tree and build a GATT database. This single GATT database is reused across all BLE connections.
The BLE Middleware manages multiple CCCD values. The CCCD value for each active connection is unique.

The maximum number of CCCD storage SRAM data structures supported by the BLE Middleware is determined by the number of 
active BLE connections/links that you select. The BLE Middleware restores CCCD values in flash for each of the bonded 
devices while establishing a connection with a peer device.

Use the connection handle at the application level to manage the multi-connection. The connection handle
(\ref cy_stc_ble_conn_handle_t) appears when the connection is established (as the event parameter of the 
\ref CY_BLE_EVT_GATT_CONNECT_IND event).

To work with a particular connection, BLE APIs (BLE Stack APIs, BLE Profile APIs) provide the parameter connHandle
(e.g., Cy_BLE_BASS_SendNotification(cy_stc_ble_conn_handle_t connHandle,... ).)
BLE events from the AppCallback function include a connHandle in eventParam structures to distinguish to which 
connection this event relates. The following code shows how an application can manage connection handles:

\code{.c}
/* Allocate the connection handle array. Macro CY_BLE_CONN_COUNT indicates the MAX number of supported connection. */
cy_stc_ble_conn_handle_t appConnHandle[CY_BLE_CONN_COUNT] = {CY_BLE_INVALID_CONN_HANDLE_VALUE};

/*  The callback event function to handle various events from the BLE stack */
void AppCallback (uint32 event, void* eventParam)
{
    switch (event)
    {
        ...
        case CY_BLE_EVT_GATT_CONNECT_IND:
        /* Add the connected device to the connection handle array */ 
        appConnHandle[ (*(cy_stc_ble_conn_handle_t *)eventParam).attId ] = 
                        *(cy_stc_ble_conn_handle_t *) eventParam; 
        break;

        case CY_BLE_EVT_GATT_DISCONNECT_IND:
        /* Remove the connected device from the connection handle array */ 
        memset(&appConnHandle[(*(cy_stc_ble_conn_handle_t*)eventParam).attId], 
               CY_BLE_INVALID_CONN_HANDLE_VALUE, 
               sizeof(cy_stc_ble_conn_handle_t));
        break;
    }
}
\endcode

Loop through all connected devices:
\code{.c}
for (i = 0; i < CY_BLE_CONN_COUNT; i++)
{
    if(Cy_BLE_GetConnectionState(appConnHandle[i]) == CY_BLE_CONN_STATE_CONNECTED)
    {
     /* Do some action */
    }
}
\endcode

Use the Cy_BLE_GetDeviceRole(cy_stc_ble_conn_handle_t *connHandle) function to discover the role of the link-layer 
device connected to a peer device with the connection handle indicated by the connHandle parameter.
Write attributes (characteristic, descriptors) requests from the peer device(s) for adopted services (e.g. BAS, HIDS,
HRS, etc) handled by the BLE Middleware.

For the Custom service, write attributes requests handled by the application level in the AppCallback () callback event 
function.
The following code shows the handling of the Write operation by a peer device for the Custom service:
\code{.c}
/* Call the back event function to handle various events from the BLE Stack */
void AppCallback (uint32 event, void* eventParam)
{
    switch (event)
    {
    ...
        case CY_BLE_EVT_GATTS_WRITE_REQ:
        {
            cy_en_ble_gatt_err_code_t gattErr;
            cy_stc_ble_gatt_write_param_t *writeParam = 
                (cy_stc_ble_gatt_write_param_t *)eventParam;

            /* Store data in the database */
            gattErr = Cy_BLE_GATTS_WriteAttributeValuePeer(
                       &writeParam->connHandle, &writeParam->handleValPair);

            if(gattErr != CY_BLE_GATT_ERR_NONE)
            {
                /* Send an Error Response */
                cy_stc_ble_gatt_err_info_t errInfo =
                {
                    .opCode     = CY_BLE_GATT_WRITE_REQ,
                    .attrHandle = writeParam->handleValPair.attrHandle,
                    .errorCode  = gattErr
                };
                Cy_BLE_GATTS_SendErrorRsp(&writeParam->connHandle, &errInfo);
            }
     ...
    }
}
\endcode

The common MMMS usage are Multi-Master Single-Slave and Multi-Role when the BLE_PDL Component is configured in all
the GAP roles (Central, Peripheral, Observer, and Broadcaster).

<b>Multi-Master Single-Slave Usage Block Diagram</b>
\htmlonly <style>div.image img[src="ble_multi_master_single_slave_usage_block_diagram.png"]{width:732;}</style> \endhtmlonly 
\image html ble_multi_master_single_slave_usage_block_diagram.png

<b>Multi-Role Usage Block Diagram</b>
\htmlonly <style>div.image img[src="ble_multi_role_usage_block_diagram.png"]{width:726;}</style> \endhtmlonly 
\image html ble_multi_role_usage_block_diagram.png

Refer to code examples CE215118_BLE_Multi_Master_Single_Slave and CE215555_BLE_Multi_Role for more details.


*
*\section group_ble_interrupt_notify BLE Interrupt Notification Callback
*
The BLE Middleware exposes BLE interrupt notifications to the application which indicates a different link layer and radio
state transitions to the user from the BLESS interrupt context.
The user registers for a particular type of a callback and the BLE component will call that registered callback basing 
on the registered mask (Refer to the Cy_BLE_RegisterInterruptCallback() and Cy_BLE_UnRegisterInterruptCallback() APIs). 
All interrupts masks are specified in the \ref cy_en_ble_interrupt_callback_feature_t enumeration.

The possible interrupts which can trigger a user callback:
    1. <b>CY_BLE_INTR_CALLBACK_BLESS_STACK_ISR</b> - Executed on every trigger of the BLESS interrupt.
    2. <b>CY_BLE_INTR_CALLBACK_BLESS_INTR_STAT_DSM_EXITED</b> - Executed when the BLESS exits deep sleep mode and enters 
    Active mode. A BLESS deep sleep exit can be triggered automatically by link layer hardware or by different BLE_PDL 
    data transfer APIs, which needs the BLESS to be active.
    3. <b>CY_BLE_INTR_CALLBACK_BLELL_CONN_EXT_INTR_EARLY</b> - Executed when the BLESS connection engine in Slave mode 
    detects a BLE packet that matches its access address. 
    4. <b>CY_BLE_INTR_CALLBACK_BLELL_CONN_INTR_CE_RX</b> - Executed when the BLESS connection engine receives a non-empty 
    packet from the peer device
    5. <b>CY_BLE_INTR_CALLBACK_BLELL_CONN_INTR_CE_TX_ACK</b> - Executed when the BLESS connection engine receives an ACK 
    packet from the peer device for the previously transmitted packet.
    6. <b>CY_BLE_INTR_CALLBACK_BLELL_CONN_INTR_CLOSE_CE</b> - Executed when the BLESS connection engine closes the 
    connection event. This interrupt is executed on every connection interval for connection, irrespective of data tx/rx 
    state.
    7. <b>CY_BLE_INTR_CALLBACK_BLESS_INTR_STAT_DSM_ENTERED</b> - Executed when the BLESS enters deep sleep mode. A user 
    call to the Cy_SysPm_DeepSleep() function will trigger the BLESS deep sleep entry sequence. A time instance when 
    each of these interrupts (#1 to #7) are triggered in a connection event are shown below (green).
    8. <b>CY_BLE_INTR_CALLBACK_BLELL_SCAN_INTR_ADV_RX</b> - Executed when the BLESS scan engine receives an advertisement 
    packet from the peer device.
    9. <b>CY_BLE_INTR_CALLBACK_BLELL_SCAN_INTR_SCAN_RSP_RX</b> - Executed when the BLESS scan engine receives a scan
    response packet from the peer device in response to a scan request from the scanner.
    10. <b>CY_BLE_INTR_CALLBACK_BLELL_ADV_INTR_CONN_REQ_RX</b> - Executed when the BLESS advertisement engine receives a 
    connection request from the peer central device.
    An application can use these interrupt callbacks to know when the RF activity is about to begin/end or when the BLE 
    device changes its state from advertisement to connected, or when the BLESS transitions between active and low-power 
    modes, etc. These BLESS real-tie states can be used to synchronize an application with the BLESS or prevent radio 
    interference with other peripherals etc.
    
    \image html ble_intr_notify_feature.png
    
This feature is enabled via define CY_BLE_INTR_NOTIFY_FEATURE_ENABLE in cy_ble_config.h. <br>
\code{.c}
#define CY_BLE_INTR_NOTIFY_FEATURE_ENABLE (1u)  //1u - Enable / 0u - Disable
\endcode

BLE Dual mode requires an additional define IPC channel and IPC Interrupt structure to send notifications from the
controller core to the host core. Use the following defines:<br>
\code{.c}
#define CY_BLE_INTR_NOTIFY_IPC_CHAN       (15u) // valid range:9..15, default:15
#define CY_BLE_INTR_NOTIFY_IPC_INTR       (15u) // valid range:9..15, default:15
#define CY_BLE_INTR_NOTIFY_IPC_INTR_PRIOR (1u)  // valid range:0..7,  default:1
\endcode
*
*\section group_ble_unsupported_features Unsupported Features
*
The BLE Middleware stack does not support the following optional Bluetooth v5.0 protocol features, as listed in 
Vol 6, Part B, section 4.6 of the specification:
    - Connection Parameters Request Procedure (Vol 6, Part B, section 4.6.2)
    - Extended Reject Indication (Vol 6, Part B, section 4.6.3)
    - Slave-initiated Features Exchange (Vol 6, Part B, section 4.6.4)
    - Stable Modulation Index - Transmitter (Vol 6, Part B, section 4.6.10)
    - Stable Modulation Index - Receiver (Vol 6, Part B, section 4.6.11)
    - LE Extended Advertising (Vol 6, Part B, section 4.6.12)
    - LE Periodic Advertising (Vol 6, Part B, section 4.6.13)
    - Channel Selection Algorithm #2 (Vol 6, Part B, section 4.6.14)
    - Minimum Number of Used Channels Procedure (Vol 6, Part B, section 4.6.15)

*
*\section group_ble_lpm Low-power Modes
*
The BLE middleware automatically registers sleep and deep sleep callback functions. This functions requests the BLE Stack 
to put Bluetooth Low Energy Sub-System (BLESS) to low-power mode. 

*
*\section group_ble_io_connections External PA/LNA Support 
*
RF front ends come in a variety of combinations. The most popular and comprehensive RF front ends come with a built-in 
power amplifier (PA) in the transmit path, a low-noise amplifier (LNA) in the receive path, a transmit/receive bypass 
path to bypass both the PA and LNA, and RF switches to select between transmit, receive, and bypass paths. Some front 
ends support multiple antennas. Some front ends have a built-in low-pass filter after the power amplifier. The discrete
front ends also have almost the same configuration. 

\image html ble_ext_pa_lna_control.png

    *\subsection group_ble_pa_lna_en Chip Enable Signal
    This signal is needed to put the front end to sleep or in standby whenever there is no radio activity. The signal is   
    ON when either PA control or LNA control is ON. 
    
    The polarity of this signal is configurable and can be set in the EXT_PA_LNA_CTRL register by 
    Cy_BLE_ConfigureExtPA() API.
    
 
    *\subsection group_ble_pa_tx_en Tx ENABLE
    This signal is turned ON during transmission and turned OFF when not transmitting. This signal is active a little 
    earlier than the actual start of transmission to allow for the time it takes for the Power amplifier to ramp up.
    This delay can be set in the EXT_PA_LNA_DLY_CNFG register.
    
    The polarity of this signal is configurable and can be set in the EXT_PA_LNA_CTRL register by Cy_BLE_ConfigureExtPA()
    API.
 
    *\subsection group_ble_lna_rx_en Rx ENABLE
    This signal is needed to choose between the bypass path and the LNA path. This signal is ON during reception and OFF
    when the receiver is OFF.
    The polarity of this signal is configurable and can be set in the EXT_PA_LNA_CTRL register by Cy_BLE_ConfigureExtPA()
    API.

    
************************************************************************************************************************
**\page page_ble_section_more_information More Information
************************************************************************************************************************
\htmlonly

<table class="doxtable">
    <tr>
        <td colspan="3"><b>Application Notes</b></td>
    </tr>
    <tr>
        <td><a href="http://www.cypress.com/an210781">AN210781</a></td>
        <td>Getting Started with PSoC 6 MCU with Bluetooth Low Energy (BLE) Connectivity</td>
        <td>Describes PSoC 6 MCU with BLE Connectivity devices and how to build your first PSoC Creator project</td>
    </tr>
    <tr>
        <td><a href="http://www.cypress.com/an215656">AN215656</a></td>
        <td>PSoC 6 MCU Dual-Core CPU System Design</td>
        <td>Presents the theory and design considerations.</td>
    </tr>
    <tr>
        <td colspan="3"><b>Software and Drivers</b></td>
    </tr>
    <tr>
        <td colspan="2"><a href="http://www.cypress.com/documentation/software-and-drivers/cysmart-bluetooth-le-test-and-debug-tool">CySmart - BLE Test and Debug Tool</a></td>
        <td>CySmart is a BLE host emulation tool for Windows PCs. The tool provides an easy-to-use GUI to enable the user to test and debug their BLE Peripheral applications.</td>
    </tr>
    <tr>
        <td colspan="3"><b>PSoC Creator Component Datasheets</b></td>
    </tr>
    <tr>
        <td colspan="2"><a href="http://www.cypress.com/documentation/component-datasheets/bluetooth-low-energy-blepdl">Bluetooth Low Energy (BLE_PDL) Component</a></td>
        <td>The Bluetooth Low Energy (BLE_PDL) Component provides a comprehensive GUI-based configuration window to facilitate designing applications requiring BLE connectivity.</td>
    </tr>
    <tr>
        <td colspan="3"><b>Device Documentation</b></td>
    </tr>
    <tr>
        <td colspan="2"><a href="http://www.cypress.com/psoc-6-63-series-datasheet">PSoC 6 MCU: PSoC 63 with BLE Datasheet Programmable System-on-Chip</a></td>
        <td><a href="http://www.cypress.com/trm218176">PSoC 6 MCU: PSoC 63 with BLE Architecture Technical Reference Manual (TRM)</a></td>
    </tr>
    <tr>
        <td colspan="3"><b>Development Kit (DVK) Documentation</b></td>
    </tr>
    <tr>
        <td colspan="3"><a href="http://www.cypress.com/cy8ckit-062-ble">CY8CKIT-062-BLE PSoC 6 BLE Pioneer Kit</a></td>
    </tr>
</table>

\endhtmlonly 
************************************************************************************************************************
**\page page_ble_section_indastry_standards Industry Standards
************************************************************************************************************************
\section section_ble_Qualify Bluetooth Qualification

BLE solutions provided by Cypress are listed on the Bluetooth SIG website as certified solutions. The qualification 
is modular, allowing greater flexibility to customers. The following is the list of Qualified Design IDs (QD ID) 
and Declaration IDs.

<table class="doxtable">
    <tr>
      <th>QD ID(s)</th>
      <th>Declaration ID#</th>
      <th>Descriptionth</th>
    </tr>
    <tr>
      <td>99158</td>
      <td><a href="https://launchstudio.bluetooth.com/ListingDetails/2312">D037716</a></td>
      <td>PSoC 6 MCU with Bluetooth Low Energy (BLE) Connectivity delivers ultra-low-power, best-in-class flexible and 
            built-in security for the Internet of Things (IoT). It's built on an ultra-low-power 40-nm process technology
            and uses a dual-core ARM® Cortex®-M architecture, which includes an Cortex-M4F and Cortex-M0+. The BLE radio
            is built using an ultra-low-power 55-nm process. PSoC 6 MCU with BLE Connectivity combines a BLE subsystem
            with software-defined analog and digital peripherals, CapSense®, programmable interconnect,
            a high-performance dual-core architecture, and critical security features in a single chip.</td>
    </tr>
</table>
    
\section section_ble_MISRA MISRA-C Compliance
This section describes the MISRA-C:2004 compliance and deviations for the BLE Middleware. 

<table class="doxtable">
    <tr>
      <th>MISRA rule</th>
      <th>Rule Class (Required/ Advisory)</th>
      <th>Rule Description</th>
      <th>Description of Deviation(s)</th>
    </tr>
    <tr>
      <td>1.2</td>
      <td>R</td>
      <td>No reliance shall be placed on undefined or unspecified behaviour.</td>
      <td>This specific behavior is explicitly covered in rule 5.1.</td>
    </tr>
    <tr>
      <td>5.1</td>
      <td>R</td>
      <td>Identifiers (internal and external) shall not rely on the significance of more than 31 characters.</td>
      <td>This rule applies to ISO:C90 standard. PDL conforms to ISO:C99 that does not require this limitation.</td>
    </tr>
    <tr>
      <td>9.3</td>
      <td>R</td>
      <td>In an enumerator list, the '=' construct shall not be used to explicitly initialise members other than the
          first, unless all items are explicitly initialised.</td>
      <td>There are enumerator lists which depend on configurations (e.g. cy_en_ble_srvi_t) and require to 
          use the '=' construct for calculating the instance count for the multi-instances services, 
          such as HIDS, BAS or CUSTOM</td>
    </tr>
    <tr>
      <td>10.1</td>
      <td>R</td>
      <td>The value of an expression of integer type shall not be implicitly converted to a different underlying type
          under some circumstances.</td>
      <td>An operand of essentially enum type is being converted to unsigned type as a result of an arithmetic or
          conditional operation. The conversion does not have any unintended effect.</td>
    </tr>
    <tr>
      <td>10.3</td>
      <td>R</td>
      <td>A composite integer expression is being cast to a wider type.</td>
      <td>Use of a Cypress defined macro to access memory-mapped objects. Calculating the clock parameters. </td>
    </tr>
    <tr>
      <td>11.4</td>
      <td>A</td>
      <td>A cast should not be performed between a pointer to object type and a different pointer to object type.</td>
      <td>A cast involving pointers is conducted with caution that the pointers are correctly aligned for the type of
          object being pointed to.</td>
    </tr>
    <tr>
      <td>11.5</td>
      <td>A</td>
      <td>A cast shall not be performed that removes any const or volatile qualification from the type addressed by a
          pointer.</td>
      <td>The const or volatile qualification is lost during pointer cast before passing to the stack functions.</td>
    </tr>
    <tr>
      <td>13.7</td>
      <td>R</td>
      <td>Boolean operations whose results are invariant shall not be permitted.</td>
      <td>A Boolean operator can yield a result that can be proven to be always "true" or always "false" in some specific
          configurations because of generalized implementation approach.</td>
    </tr>
    <tr>
      <td>16.7</td>
      <td>A</td>
      <td>The object addressed by the pointer parameter is not modified and so the pointer could be of type 'pointer to
            const'.</td>
      <td>The 'base' and 'content' parameters in Cy_BLE_DeepSleepCallback function are not used by BLE but callback type 
            is universal for all drivers.</td>
    </tr>
    <tr>
      <td>18.4</td>
      <td>R</td>
      <td>Unions shall not be used.</td>
      <td>The use of deviations is acceptable for packing and unpacking of data, for example when sending and 
          receiving messages, and implementing variant records provided that the variants are differentiated 
          by a common field. </td>
    </tr>
    <tr>
      <td>19.16</td>
      <td>R</td>
      <td>Preprocessing directives shall be syntactically meaningful even when excluded by the preprocessor.</td>
      <td>The reason for this deviation is that not standard directive "warning" is used. This directive is works 
          on all compilers which PDL supports (e.g. GCC, IAR, MDK). </td>
    </tr>
</table>

************************************************************************************************************************
**\page page_group_ble_changelog Changelog
************************************************************************************************************************
*\section section_group_ble_changelog_BLE_Middleware_Changes BLE Middleware Changes
    This section lists changes made to the BLE Middleware.
<table class="doxtable">
    <tr><th><b>Version</b></th><th><b>Changes</b></th><th><b>Reason for Change</b></th></tr>  
        
    <tr>
        <td rowspan="6">2.20</td>
        <td>Added support for PILO. </td>
        <td>PSoC 6 BLE supports Precision ILO (PILO) which can be used as an LF clock source. 
            Refer to \ref group_ble_lfclk_conf for more details. </td>
    </tr>
    <tr>
        <td>Updated the Cy_BLE_RegisterAppHostCallback() functionality.</td>
        <td>Added support for BLE Single mode. </td>
    </tr>
    <tr>
        <td>Updated Write Request events handler in BLE middleware. Return CY_BLE_GATT_ERR_INVALID_HANDLE if 
            the attribute handle is out of database range. </td>
        <td>Handling invalid range of attribute handle during Write Request event. </td>
    </tr>
    <tr>
        <td>Updated flash memory calculation for Bonded device list </td>
        <td>The function Cy_BLE_StackInit() returned CY_BLE_ERROR_INVALID_PARAMETER when the Resolving list 
            parameter is set to 1. </td>
    </tr>
    <tr>
        <td>Documentation updates. </td>
        <td>Documentation update and clarification. Removed the Errata section. </td>
    </tr>
    <tr>
        <td>Updated the BLE Stack to version 5.0.2.917.</td>
        <td>Refer to \ref section_group_ble_changelog_BLE_Stack_Changes</td>
    </tr>

    <tr>
        <td rowspan="2">2.10</td>
        <td>Added return value CY_BLE_ERROR_FLASH_WRITE_NOT_PERMITED for following functions:
            Cy_BLE_StoreAppData()<br>Cy_BLE_StoreBondingData()<br>
            Added assertion in Cy_BLE_HAL_NvramWrite() to catch failure (CY_SYSPM_INVALID_STATE) during increasing 
            core voltage from 0.9V to 1.1V for Flash operation.
        </td>
        <td>Flash operation is not permitted with protection context (PC) value > 0 and core voltage 0.9V,
            because of a preproduction hardware limitation.
        </td>
    </tr>
    <tr>
        <td>Documentation updates. </td>
        <td>Documentation update and clarification. </td>
    </tr>
    
    <tr>
        <td rowspan="5">2.0<br> Production</td>
        <td>Added the BLE user configuration file cy_ble_config.h. </td>
        <td>To make the BLE configuration more flexible.
            Allows redefining the configuration define(s) generated by the BLE customizer 
            and default BLE clock defines (from cy_ble_clk.h).
        </td>
    </tr>
    <tr>
        <td>Updated the BLE Interrupt Notification Feature. </td>
        <td>Added support for BLE Dual mode.</td>
    </tr>
    <tr>
        <td>Updated the Cy_BLE_GATTS_ReadAttributeValueLocal() and Cy_BLE_GATTS_ReadAttributeValuePeer() functions. </td>
        <td>Those functions have the wrong Locally/Peer initiate operation flag.</td>
    </tr>
    <tr>
        <td>The following defined values for the CY_BLE_STACK_MODE 
                configuration parameter were deprecated:<br>
                CY_BLE_STACK_HOST_ONLY<br>
                CY_BLE_STACK_HOST_CONTR_UART<br>
                CY_BLE_STACK_HOST_IPC.<br>
            The following defined values were renamed:<br>
                CY_BLE_STACK_DEBUG to CY_BLE_STACK_MODE_SINGLE_SOC <br>
                CY_BLE_STACK_RELEASE to CY_BLE_STACK_MODE_DUAL_IPC.<br>
        </td>
        <td>Deprecated not supported (debug) modes.<br>
            The defines CY_BLE_CONFIG_STACK_DEBUG and CY_BLE_CONFIG_STACK_RELEASE were renamed to have more meaningful name.
        </td>
    </tr>
    <tr>
        <td>The BLE Stack was updated to version 5.0.0.898. </td>
        <td>Refer to \ref section_group_ble_changelog_BLE_Stack_Changes</td>
    </tr>
    
    <tr>
        <td rowspan="2" >2.0<br> BETA#2</td>
        <td>Added new API: <br> Cy_BLE_IsDevicePaired(),<br> Cy_BLE_GetDeviceRole(),<br> Cy_BLE_IsPeerConnected(),<br>
            Cy_BLE_GATTS_WriteAttributeValuePeer(),<br> Cy_BLE_GATTS_WriteAttributeValueLocal(),<br> 
            Cy_BLE_GATTS_ReadAttributeValuePeer(),<br> Cy_BLE_GATTS_ReadAttributeValueLocal(),<br> Cy_BLE_GATTS_IsIndicationEnabled(),<br>
            Cy_BLE_GATTS_SendIndication(),<br> Cy_BLE_GATTS_IsNotificationEnabled(),<br> Cy_BLE_GATTS_SendNotification(),<br>
            Cy_BLE_GATTS_SendErrorRsp(),<br> Cy_BLE_GATTC_SendConfirmation().
        </td>
        <td>Improve usability</td>
    </tr>
    <tr>
        <td>The BLE Stack was updated to version 5.0.0.855.</td>
        <td>Refer to \ref section_group_ble_changelog_BLE_Stack_Changes</td>
    </tr>
    
    <tr>
        <td rowspan="4" >2.0 <br> BETA#1</td>
        <td>Renamed API: <br>
            Cy_BLE_GAP_UpdateAdvScanData() to Cy_BLE_GAPP_UpdateAdvScanData(); <br>
            Cy_BLE_SetConnectionPriority() to Cy_BLE_GAP_SetConnectionPriority(). <br>
        </td>
        <td>Consistent API naming scheme</td>
    </tr>
    <tr>
        <td>Made the maximum number of connections configurable.</td>
        <td>SRAM consumption reducing when extra connections are not used.</td>
    </tr>
    <tr>
        <td>Added new options for the CPU Core parameter.</td>
        <td>Support of single core devices.
    </tr>
    <tr>
        <td>The BLE Stack was updated to version 5.0.0.785.</td>
        <td>Refer to \ref section_group_ble_changelog_BLE_Stack_Changes</td>
    </tr>
    
    <tr>
        <td>1.0</td>
        <td>Initial version.</td>
        <td></td>
    </tr>
</table>
    
*\section section_group_ble_changelog_BLE_Stack_Changes BLE Stack Changes
    This section lists changes made to the BLE Stack.

<table class="doxtable">
    <tr><th><b>Version</b></th><th><b>Changes</b></th><th><b>Reason for Change</b></th></tr>
    
    <tr>
        <td rowspan="3">5.0.2.917</td>
        <td>Added a new BLE event - CY_BLE_EVT_GAP_ADV_TX, which is triggered on transmission of an ADV packet (if this 
            feature is enabled). </td>
        <td>Support for BLE mesh.</td>
    </tr>
    <tr>
        <td>Dual Core: Stack Shutdown sequence modified to shut down controller before 
            sending CY_BLE_EVT_STACK_SHUTDOWN_COMPLETE event. </td>
        <td>The controller was shut-down after informing the application that shutdown had been completed. </td>
    </tr>
    <tr>
        <td>Updated the Cy_BLE_HAL_BlessInterruptHandler API to return status. </td>
        <td>This return parameter indicates that user should call Cy_BLE_ProcessEvents() to process pending BLE Stack 
            events.</td>
    </tr>
    
    <tr>
        <td rowspan="4">5.0.1.906</td>
        <td>Added check for any pending real time procedures before allowing autonomous PHY update request. </td>
        <td>Autonomous PHY update request was being initiated without a check for "LL control procedure collision". </td>
    </tr>
    <tr>
        <td>Fixed memory allocation failure issue observed when initializing buffer pool for ACL Rx Packet. </td>
        <td>Memory allocation failure was observed when DLE Rx Octet < TxOctet. </td>
    </tr>
    <tr>
        <td>Added support to disconnect LE link whenever there is real-time LL procedure collision. </td>
        <td>Consistent with BLE Spec 5.0 erratum 7106</td>
    </tr>
    <tr>
        <td>Added support for PILO. </td>
        <td>PSoC 6 BLE supports a Precision ILO which can be used as an LF clock source instead of the WCO. </td>
    </tr>
    
    <tr>
        <td rowspan="2">5.0.0.898</td>
        <td>Enhancement is done to rerun the scheduler on close CE, if the event about to be scheduled is in past. </td>
        <td>Blocking flash write operation blocks the CPU from executing any interrupts (including BLESS isr) for > 17ms,
            Due to this, if the CI for connection is smaller than 17ms, the BLE link will disconnect.
        </td>
    </tr>
    <tr>
        <td>The calibration target value for the Max output power level is modified under the user-defined condition.</td>
        <td>The enhancement to support the 5dBm Tx power in the BLE stack.</td>
    </tr>
    
    <tr>
        <td rowspan="3">5.0.0.855</td>
        <td>Changed the input parameter for the Cy_BLE_SetCustomEventMask() API from (uint8* mask) to (uint32 mask).</td>
        <td>Consistent with the bit mask scheme. </td>
    </tr>
    <tr>
        <td>1.The timer context was never reset, it is made to reset during the Stack shutdown. <br>
            2.The timer context was not freed even when the timer start failed. It is made free when the timer start 
            fails due to any reason.
            </td>
        <td>The Cy_BLE_GAPC_StopScan() function returns CY_BLE_ERROR_MAX when the Cy_BLE_GAPC_StopScan() function is 
            called to stop a previously-initiated scanning procedure.
            This issue occurs only when the authentication procedure is occurring and the application tries to stop 
            the scan </td>
    </tr>
    <tr>
        <td>1.The DLE control procedure timer is started only if the queuing Data Length Request in the Hardware FIFO 
            is successful.<BR>
            2.Added checks to handle the race condition between the ACK processing and Handling data PDU's from the Host.
        </td>
        <td>Unexpected disconnection due to an LMP Response timeout in the embedded stress application. This issue is 
            due to:<BR>
            1.The DLE control procedure timer was started even if the sending Data Length Request PDU failed when queuing 
            it in the Hardware FIFO.<BR>
            2.Due to the race condition between the ACK processing and Handling data PDU's from the Host.
        </td>
    </tr>
    
    <tr>
        <td>5.0.0.785</td>
        <td>Initial BLE Stack version. </td>
        <td> </td>
    </tr>
</table>    


************************************************************************************************************************
* \defgroup group_ble_common_api BLE Common API
************************************************************************************************************************
* \ingroup group_ble
*
* \brief
* The common API act as a general interface between the BLE application
* and the BLE Stack module. The application may use these API to control
* the underlying hardware such as radio power, data encryption and device
* bonding via the stack. It may also access the GAP, GATT and L2CAP layers
* of the stack. These are divided into the following categories:
* - \ref group_ble_common_api_functions
* - \ref group_ble_common_api_gap_functions_section
* - \ref group_ble_common_api_gatt_functions_section
* - \ref group_ble_common_api_l2cap_functions
*
* These API also use API specific definitions and data structures. These are classified
* in the following subset:
* - \ref group_ble_common_api_definitions
*
*
* \defgroup group_ble_common_api_functions BLE Common Core Functions
* \ingroup group_ble_common_api
* \brief
* The common core API are used for general BLE configuration.
* These include initialization, power management, and utilities.
*
*
* \defgroup group_ble_common_api_creator_functions PSoC Creator API
* \ingroup group_ble_common_api_functions
* \brief
* These API are provided by the PSoC Creator IDE and used to easily start/stop BLE.
*
* \defgroup group_ble_common_Whitelist_api_functions Whitelist API
* \ingroup group_ble_common_api_functions
* \brief
* The API are used for enable user to use Whitelist feature of BLE Stack.
*
* \defgroup group_ble_common_Privacy_api_functions Link Layer Privacy API
* \ingroup group_ble_common_api_functions
* \brief
* The API are used for enable user to use Link Layer Privacy feature of BLE Stack.
*
* \defgroup group_ble_common_Data_length_extension_api_functions Data Length Extension (DLE) API
* \ingroup group_ble_common_api_functions
* \brief
* The API are used for enable user to use Data Length Extension (DLE) feature of BLE Stack.
* 
* \defgroup group_ble_common_2MBPS_api_functions 2Mbps Feature API
* \ingroup group_ble_common_api_functions
* \brief
* The API are used for enable user to use 2Mbps feature of BLE Stack.
*
* \defgroup group_ble_common_LE_ping_api_functions LE Ping API
* \ingroup group_ble_common_api_functions
* \brief
* The API are used for enable user to use LE Ping feature of BLE Stack.
*
* \defgroup group_ble_common_Encryption_api_functions AES Engine API
* \ingroup group_ble_common_api_functions
* \brief
*  BLE sub system AES Engine is exposed through this API.
*
* \defgroup group_ble_common_HCI_api_functions BLE HCI API
* \ingroup group_ble_common_api_functions
* \brief
* API exposes BLE Stack's Host HCI to user, if they want to do DTM testing or use BLE Controller alone.
*
* \defgroup group_ble_common_api_gap_functions_section GAP Functions
* \ingroup group_ble_common_api
* \brief
* The GAP APIs allow access to the Generic Access Profile (GAP) layer 
* of the BLE Stack. Depending on the chosen GAP role in the GUI, you may
* use a subset of the supported APIs.
* 
* The GAP API names begin with Cy_BLE_GAP_. In addition to this, the APIs
* also append the GAP role initial letter in the API name.
* 
* \defgroup group_ble_common_api_gap_functions GAP Central and Peripheral Functions
* \ingroup group_ble_common_api_gap_functions_section
* \brief
* These are APIs common to both GAP Central role and GAP Peripheral role. 
* You may use them in either roles.
* 
* No letter is appended to the API name: Cy_BLE_GAP_
* 
* \defgroup group_ble_common_api_gap_central_functions GAP Central Functions
* \ingroup group_ble_common_api_gap_functions_section
* \brief
* APIs unique to designs configured as a GAP Central role. 
* 
* A letter 'C' is appended to the API name: Cy_BLE_GAPC_
* 
* \defgroup group_ble_common_api_gap_peripheral_functions GAP Peripheral Functions
* \ingroup group_ble_common_api_gap_functions_section
* \brief
* APIs unique to designs configured as a GAP Peripheral role. 
* 
* A letter 'P' is appended to the API name: Cy_BLE_GAPP_
* 
* 
* \defgroup group_ble_common_api_gatt_functions_section GATT Functions
* \ingroup group_ble_common_api
* \brief
* The GATT APIs allow access to the Generic Attribute Profile (GATT) layer 
* of the BLE Stack. Depending on the chosen GATT role in the GUI, you may
* use a subset of the supported APIs.
* 
* The GATT API names begin with CyBle_Gatt. In addition to this, the APIs
* also append the GATT role initial letter in the API name.
* 
* \defgroup group_ble_common_api_gatt_functions GATT Client and Server Functions
* \ingroup group_ble_common_api_gatt_functions_section
* \brief
* These are APIs common to both GATT Client role and GATT Server role. 
* You may use them in either roles.
* 
* No letter is appended to the API name: Cy_BLE_GATT_
* 
* \defgroup group_ble_common_api_gatt_client_functions GATT Client Functions
* \ingroup group_ble_common_api_gatt_functions_section
* \brief
* APIs unique to designs configured as a GATT Client role. 
* 
* A letter 'C' is appended to the API name: Cy_BLE_GATTC_
* 
* \defgroup group_ble_common_api_gatt_server_functions GATT Server Functions
* \ingroup group_ble_common_api_gatt_functions_section
* \brief
* APIs unique to designs configured as a GATT Server role. 
* 
* A letter 'S' is appended to the API name: Cy_BLE_GATTS_
* 
* 
* \defgroup group_ble_common_api_l2cap_functions L2CAP Functions
* \ingroup group_ble_common_api
* \brief
* The L2CAP APIs allow access to the Logical link control and adaptation
* protocol (L2CAP) layer of the BLE Stack.
* 
* The L2CAP API names begin with Cy_BLE_L2CAP.
* 
* 
*  
* \defgroup group_ble_common_api_events BLE Common Events
* \ingroup group_ble_common_api
* \brief
* The BLE Stack generates events to notify the application on various status
* alerts concerning the stack. These can be generic stack events or can be 
* specific to GAP, GATT or L2CAP layers. The service specific events are
* handled separately in \ref group_ble_service_api_events.
*  
*
* \defgroup group_ble_common_api_definitions_section BLE Common Definitions and Data Structures
* \ingroup group_ble_common_api
* \brief
* Contains definitions and structures that are common to all BLE common API.
* Note that some of these are also used in Service-specific API.
*
* \defgroup group_ble_common_macros_error BLE Error Code
* \ingroup group_ble_common_api_definitions_section
* \brief
* Contains definitions for all the spec defined error code in Core Spec 5.0, Vol2, Part D
* 
* Related Document:
*  BLE Standard Spec - CoreV5.0
*
* \defgroup group_ble_common_api_macros_section Macros
* \ingroup group_ble_common_api_definitions_section
*
* \defgroup group_ble_common_api_macros Common
* \ingroup group_ble_common_api_macros_section
*  BLE Common macros
*
* \defgroup group_ble_common_api_macros_gatt_db BLE GATT Database
* \ingroup group_ble_common_api_macros_section
* \brief
*  BLE GATT Database macros
*
* \defgroup group_ble_common_api_macros_gatt_uuid_services BLE Services UUID 
* \ingroup group_ble_common_api_macros_section
* \brief
*   BLE Services Universal Unique Identifier (UUID) macros
*
* \defgroup group_ble_common_api_macros_gatt_uuid_char_gatt_type BLE GATT Attribute Types UUID 
* \ingroup group_ble_common_api_macros_section
* \brief
*   BLE GATT Attribute Types defined by GATT Profile UUID macros
*
* \defgroup group_ble_common_api_macros_gatt_uuid_char_desc BLE GATT Characteristic Descriptors UUID 
* \ingroup group_ble_common_api_macros_section
* \brief
*   BLE GATT Attribute Types defined by GATT Profile UUID macros
*
* \defgroup group_ble_common_api_macros_gatt_uuid_char_type BLE GATT Characteristic Types UUID 
* \ingroup group_ble_common_api_macros_section
* \brief
*   BLE GATT Characteristic Types UUID macros
*
* \defgroup group_ble_common_api_macros_appearance_values BLE Appearance values
* \ingroup group_ble_common_api_macros_section
* \brief
*   BLE Appearance values macros
*           
* \defgroup group_ble_common_api_data_struct_section Data Structures
* \ingroup group_ble_common_api_definitions_section
*
* \defgroup group_ble_common_api_definitions Common 
* \ingroup group_ble_common_api_data_struct_section
* \brief
* Contains the common definitions and data structures used in the BLE.
*
* \defgroup group_ble_common_api_gap_definitions GAP 
* \ingroup group_ble_common_api_data_struct_section
* \brief
* Contains the GAP specific definitions and data structures used in the
* GAP APIs.
*
* \defgroup group_ble_common_api_gatt_definitions GATT
* \ingroup group_ble_common_api_data_struct_section
* \brief
* Contains the GATT specific definitions and data structures used in the
* GATT APIs.
*
* \defgroup group_ble_common_api_l2cap_definitions L2CAP 
* \ingroup group_ble_common_api_data_struct_section
* \brief
* Contains the L2CAP specific definitions and data structures used in the
* L2CAP APIs.
*
* \defgroup group_ble_common_api_global_variables Global Variables 
* \ingroup group_ble_common_api
* \defgroup group_ble_service_api BLE Service-Specific API
* \ingroup group_ble
* \brief
* This section describes BLE Service-specific API. The Service
* API are only included in the design if the Service is
* added to the selected Profile in the BLE GUI.
* These are interfaces for the BLE application to use during BLE
* connectivity. The service-specific API internally use the BLE
* Stack API to achieve the Service use case.
* Refer to the <a href="https://www.bluetooth.org/en-us/specification/adopted-specifications">Special Interest Group
* Web Site</a>
* for links to the latest specifications and other documentation.
*
* Many of the API will generate Service-specific events. The events
* are also used in the Service-specific callback functions. These are
* documented in:
* - \ref group_ble_service_api_events
*
*
* \defgroup group_ble_service_api_events BLE Service-Specific Events
* \ingroup group_ble_service_api
* \brief
* The BLE Stack generates service-specific events to notify the application
* that a service-specific status change needs attention.
*
*
*
* \defgroup group_ble_service_api_ANCS Apple Notification Center Service (ANCS)
* \ingroup group_ble_service_api
* \brief
* The Apple Notification Center Service provides iOS notifications from Apple devices for accessories.
*
* Depending on the chosen GATT role in the GUI, you may
* use a subset of the supported API.
*
* The ANCS API names begin with Cy_BLE_ANCS_. In addition to this, the API
* also append the GATT role initial letter in the API name.
*
* \defgroup group_ble_service_api_ANCS_server_client ANCS Server and Client Function
* \ingroup group_ble_service_api_ANCS
* \brief
* These are API common to both GATT Client role and GATT Server role.
* You may use them in either roles.
*
* No letter is appended to the API name: Cy_BLE_ANCS_
*
* \defgroup group_ble_service_api_ANCS_server ANCS Server Functions
* \ingroup group_ble_service_api_ANCS
* \brief
* API unique to ANCS designs configured as a GATT Server role.
*
* A letter 's' is appended to the API name: Cy_BLE_ANCSS_
*
* \defgroup group_ble_service_api_ANCS_client ANCS Client Functions
* \ingroup group_ble_service_api_ANCS
* \brief
* API unique to ANCS designs configured as a GATT Client role.
*
* A letter 'c' is appended to the API name: Cy_BLE_ANCSC_
*
* \defgroup group_ble_service_api_ANCS_definitions ANCS Definitions and Data Structures
* \ingroup group_ble_service_api_ANCS
* \brief
* Contains the ANCS specific definitions and data structures used
* in the ANCS API.
*
*
*
* \defgroup group_ble_service_api_ANS Alert Notification Service (ANS)
* \ingroup group_ble_service_api
* \brief
* The Alert Notification Service exposes alert information in a device.
*
* This information includes:
* - Type of alert occurring in a device
* - Additional text information such as the caller's ID or sender's ID
* - Count of new alerts
* - Count of unread alert items
*
* Depending on the chosen GATT role in the GUI, you may
* use a subset of the supported API.
*
* The ANS API names begin with Cy_BLE_ANS_. In addition to this, the API
* also append the GATT role initial letter in the API name.
*
* \defgroup group_ble_service_api_ANS_server_client ANS Server and Client Function
* \ingroup group_ble_service_api_ANS
* \brief
* These are API common to both GATT Client role and GATT Server role.
* You may use them in either roles.
*
* No letter is appended to the API name: Cy_BLE_ANS_
*
* \defgroup group_ble_service_api_ANS_server ANS Server Functions
* \ingroup group_ble_service_api_ANS
* \brief
* API unique to ANS designs configured as a GATT Server role.
*
* A letter 's' is appended to the API name: Cy_BLE_ANSS_
*
* \defgroup group_ble_service_api_ANS_client ANS Client Functions
* \ingroup group_ble_service_api_ANS
* \brief
* API unique to ANS designs configured as a GATT Client role.
*
* A letter 'c' is appended to the API name: Cy_BLE_ANSC_
*
* \defgroup group_ble_service_api_ANS_definitions ANS Definitions and Data Structures
* \ingroup group_ble_service_api_ANS
* \brief
* Contains the ANS specific definitions and data structures used
* in the ANS API.
*
*
*
* \defgroup group_ble_service_api_AIOS Automation IO Service (AIOS)
* \ingroup group_ble_service_api
* \brief
* The Automation IO Service enables a device to connect and interact with an Automation IO Module (IOM) in order to access digital and analog signals.
*
* Depending on the chosen GATT role in the GUI, you may
* use a subset of the supported API.
*
* The AIOS API names begin with Cy_BLE_AIOS_. In addition to this, the API
* also append the GATT role initial letter in the API name.
*
* \defgroup group_ble_service_api_AIOS_server_client AIOS Server and Client Function
* \ingroup group_ble_service_api_AIOS
* \brief
* These are API common to both GATT Client role and GATT Server role.
* You may use them in either roles.
*
* No letter is appended to the API name: Cy_BLE_AIOS_
*
* \defgroup group_ble_service_api_AIOS_server AIOS Server Functions
* \ingroup group_ble_service_api_AIOS
* \brief
* API unique to AIOS designs configured as a GATT Server role.
*
* A letter 's' is appended to the API name: Cy_BLE_AIOSS_
*
* \defgroup group_ble_service_api_AIOS_client AIOS Client Functions
* \ingroup group_ble_service_api_AIOS
* \brief
* API unique to AIOS designs configured as a GATT Client role.
*
* A letter 'c' is appended to the API name: Cy_BLE_AIOSC_
*
* \defgroup group_ble_service_api_AIOS_definitions AIOS Definitions and Data Structures
* \ingroup group_ble_service_api_AIOS
* \brief
* Contains the AIOS specific definitions and data structures used
* in the AIOS API.
*
*
* \defgroup group_ble_service_api_BAS Battery Service (BAS)
* \ingroup group_ble_service_api
* \brief
* The Battery Service exposes the battery level of a single battery
* or set of batteries in a device.
*
* Depending on the chosen GATT role in the GUI, you may
* use a subset of the supported API.
*
* The BAS API names begin with Cy_BLE_BAS_. In addition to this, the API
* also append the GATT role initial letter in the API name.
*
* \defgroup group_ble_service_api_BAS_server_client BAS Server and Client Function
* \ingroup group_ble_service_api_BAS
* \brief
* These are API common to both GATT Client role and GATT Server role.
* You may use them in either roles.
*
* No letter is appended to the API name: Cy_BLE_BAS_
*
* \defgroup group_ble_service_api_BAS_server BAS Server Functions
* \ingroup group_ble_service_api_BAS
* \brief
* API unique to BAS designs configured as a GATT Server role.
*
* A letter 's' is appended to the API name: Cy_BLE_BASS_
*
* \defgroup group_ble_service_api_BAS_client BAS Client Functions
* \ingroup group_ble_service_api_BAS
* \brief
* API unique to BAS designs configured as a GATT Client role.
*
* A letter 'c' is appended to the API name: Cy_BLE_BASC_
*
* \defgroup group_ble_service_api_BAS_definitions BAS Definitions and Data Structures
* \ingroup group_ble_service_api_BAS
* \brief
* Contains the BAS specific definitions and data structures used
* in the BAS API.
*
*
*
* \defgroup group_ble_service_api_BCS Body Composition Service (BCS)
* \ingroup group_ble_service_api
* \brief
* The Body Composition Service exposes data related to body composition from a body composition analyzer (Server)
* intended for consumer healthcare as well as sports/fitness applications.
*
* Depending on the chosen GATT role in the GUI, you may use a subset of the supported API.
*
* The BCS API names begin with Cy_BLE_BCS_. In addition to this, the API also append the GATT role initial letter in the
* API name.
*
* \defgroup group_ble_service_api_BCS_server_client BCS Server and Client Function
* \ingroup group_ble_service_api_BCS
* \brief
* These are API common to both GATT Client role and GATT Server role. You may use them in either roles.
*
* No letter is appended to the API name: Cy_BLE_BCS_
*
* \defgroup group_ble_service_api_BCS_server BCS Server Functions
* \ingroup group_ble_service_api_BCS
* \brief
* API unique to BCS designs configured as a GATT Server role.
*
* A letter 's' is appended to the API name: Cy_BLE_BCSS_
*
* \defgroup group_ble_service_api_BCS_client BCS Client Functions
* \ingroup group_ble_service_api_BCS
* \brief
* API unique to BCS designs configured as a GATT Client role.
*
* A letter 'c' is appended to the API name: Cy_BLE_BCSC_
*
* \defgroup group_ble_service_api_BCS_definitions BCS Definitions and Data Structures
* \ingroup group_ble_service_api_BCS
* \brief
* Contains the BCS specific definitions and data structures used in the BCS API.
*
*
*
* \defgroup group_ble_service_api_BLS Blood Pressure Service (BLS)
* \ingroup group_ble_service_api
* \brief
* The Blood Pressure Service exposes blood pressure and other data related
* to a non-invasive blood pressure monitor for consumer and professional
* healthcare applications.
*
* Depending on the chosen GATT role in the GUI, you may
* use a subset of the supported API.
*
* The BLS API names begin with Cy_BLE_BLS_. In addition to this, the API
* also append the GATT role initial letter in the API name.
*
* \defgroup group_ble_service_api_BLS_server_client BLS Server and Client Function
* \ingroup group_ble_service_api_BLS
* \brief
* These are API common to both GATT Client role and GATT Server role.
* You may use them in either roles.
*
* No letter is appended to the API name: Cy_BLE_BLS_
*
* \defgroup group_ble_service_api_BLS_server BLS Server Functions
* \ingroup group_ble_service_api_BLS
* \brief
* API unique to BLS designs configured as a GATT Server role.
*
* A letter 's' is appended to the API name: Cy_BLE_BLSS_
*
* \defgroup group_ble_service_api_BLS_client BLS Client Functions
* \ingroup group_ble_service_api_BLS
* \brief
* API unique to BLS designs configured as a GATT Client role.
*
* A letter 'c' is appended to the API name: Cy_BLE_BLSC_
*
* \defgroup group_ble_service_api_BLS_definitions BLS Definitions and Data Structures
* \ingroup group_ble_service_api_BLS
* \brief
* Contains the BLS specific definitions and data structures
* used in the BLS API.
*
*
*
* \defgroup group_ble_service_api_BMS Bond Management Service (BMS)
* \ingroup group_ble_service_api
* \brief
* The Bond Management Service defines how a peer Bluetooth device can manage the storage of bond information, especially
* the deletion of it, on the Bluetooth device supporting this service.
*
* Depending on the chosen GATT role in the GUI, you may use a subset of the supported API.
*
* The BMS API names begin with Cy_BLE_BMS_. In addition to this, the API also append the GATT role initial letter in the
* API name.
*
* \defgroup group_ble_service_api_BMS_server_client BMS Server and Client Function
* \ingroup group_ble_service_api_BMS
* \brief
* These are API common to both GATT Client role and GATT Server role. You may use them in either roles.
*
* No letter is appended to the API name: Cy_BLE_BMS_
*
* \defgroup group_ble_service_api_BMS_server BMS Server Functions
* \ingroup group_ble_service_api_BMS
* \brief
* API unique to BMS designs configured as a GATT Server role.
*
* A letter 's' is appended to the API name: Cy_BLE_BMSS_
*
* \defgroup group_ble_service_api_BMS_client BMS Client Functions
* \ingroup group_ble_service_api_BMS
* \brief
* API unique to BMS designs configured as a GATT Client role.
*
* A letter 'c' is appended to the API name: Cy_BLE_BMSC_
*
* \defgroup group_ble_service_api_BMS_definitions BMS Definitions and Data Structures
* \ingroup group_ble_service_api_BMS
* \brief
* Contains the BMS specific definitions and data structures used in the BMS API.
*
*
*
* \defgroup group_ble_service_api_CGMS Continuous Glucose Monitoring Service (CGMS)
* \ingroup group_ble_service_api
* \brief
* The Continuous Glucose Monitoring Service exposes glucose measurement and other data related to a personal CGM sensor
* for healthcare applications.
*
* Depending on the chosen GATT role in the GUI, you may use a subset of the supported API.
*
* The CGMS API names begin with Cy_BLE_CGMS_. In addition to this, the API also append the GATT role initial letter in the
* API name.
*
* \defgroup group_ble_service_api_CGMS_server_client CGMS Server and Client Function
* \ingroup group_ble_service_api_CGMS
* \brief
* These are API common to both GATT Client role and GATT Server role. You may use them in either roles.
*
* No letter is appended to the API name: Cy_BLE_CGMS_
*
* \defgroup group_ble_service_api_CGMS_server CGMS Server Functions
* \ingroup group_ble_service_api_CGMS
* \brief
* API unique to CGMS designs configured as a GATT Server role.
*
* A letter 's' is appended to the API name: Cy_BLE_CGMSS_
*
* \defgroup group_ble_service_api_CGMS_client CGMS Client Functions
* \ingroup group_ble_service_api_CGMS
* \brief
* API unique to CGMS designs configured as a GATT Client role.
*
* A letter 'c' is appended to the API name: Cy_BLE_CGMSC_
*
* \defgroup group_ble_service_api_CGMS_definitions CGMS Definitions and Data Structures
* \ingroup group_ble_service_api_CGMS
* \brief
* Contains the CGMS specific definitions and data structures used in the CGMS API.
*
*
*
* \defgroup group_ble_service_api_CPS Cycling Power Service (CPS)
* \ingroup group_ble_service_api
* \brief
* The Cycling Power Service (CPS) exposes power- and force-related
* data and optionally speed- and cadence-related data from a Cycling
* Power sensor (GATT Server) intended for sports and fitness applications.
*
* Depending on the chosen GATT role in the GUI, you may
* use a subset of the supported API.
*
* The CPS API names begin with Cy_BLE_CPS_. In addition to this, the API
* also append the GATT role initial letter in the API name.
*
*
* \defgroup group_ble_service_api_CPS_server_client CPS Server and Client Function
* \ingroup group_ble_service_api_CPS
* \brief
* These are API common to both GATT Client role and GATT Server role.
* You may use them in either roles.
*
* No letter is appended to the API name: Cy_BLE_CPS_
*
* \defgroup group_ble_service_api_CPS_server CPS Server Functions
* \ingroup group_ble_service_api_CPS
* \brief
* API unique to CPS designs configured as a GATT Server role.
*
* A letter 's' is appended to the API name: Cy_BLE_CPSS_
*
* \defgroup group_ble_service_api_CPS_client CPS Client Functions
* \ingroup group_ble_service_api_CPS
* \brief
* API unique to CPS designs configured as a GATT Client role.
*
* A letter 'c' is appended to the API name: Cy_BLE_CPSC_
*
* \defgroup group_ble_service_api_CPS_definitions CPS Definitions and Data Structures
* \ingroup group_ble_service_api_CPS
* \brief
* Contains the CPS specific definitions and data structures
* used in the CPS API.
*
*
* \defgroup group_ble_service_api_CSCS Cycling Speed and Cadence Service (CSCS)
* \ingroup group_ble_service_api
* \brief
* The Cycling Speed and Cadence (CSC) Service exposes speed-related
* data and/or cadence-related data while using the Cycling Speed
* and Cadence sensor (Server).
*
* Depending on the chosen GATT role in the GUI, you may
* use a subset of the supported API.
*
* The CSCS API names begin with Cy_BLE_CSCS_. In addition to this, the API
* also append the GATT role initial letter in the API name.
*
*
* \defgroup group_ble_service_api_CSCS_server_client CSCS Server and Client Function
* \ingroup group_ble_service_api_CSCS
* \brief
* These are API common to both GATT Client role and GATT Server role.
* You may use them in either roles.
*
* No letter is appended to the API name: Cy_BLE_CSCS_
*
* \defgroup group_ble_service_api_CSCS_server CSCS Server Functions
* \ingroup group_ble_service_api_CSCS
* \brief
* API unique to CSCS designs configured as a GATT Server role.
*
* A letter 's' is appended to the API name: Cy_BLE_CSCSS_
*
* \defgroup group_ble_service_api_CSCS_client CSCS Client Functions
* \ingroup group_ble_service_api_CSCS
* \brief
* API unique to CSCS designs configured as a GATT Client role.
*
* A letter 'c' is appended to the API name: Cy_BLE_CSCSC_
*
* \defgroup group_ble_service_api_CSCS_definitions CSCS Definitions and Data Structures
* \ingroup group_ble_service_api_CSCS
* \brief
* Contains the CSCS specific definitions and data structures
* used in the CSCS API.
*
*
* \defgroup group_ble_service_api_CTS Current Time Service (CTS)
* \ingroup group_ble_service_api
* \brief
* The Current Time Service defines how a Bluetooth device can expose time information
* to other Bluetooth devices.
*
* Depending on the chosen GATT role in the GUI, you may
* use a subset of the supported API.
*
* The CTS API names begin with Cy_BLE_CTS_. In addition to this, the API
* also append the GATT role initial letter in the API name.
*
* \defgroup group_ble_service_api_CTS_server_client CTS Server and Client Function
* \ingroup group_ble_service_api_CTS
* \brief
* These are API common to both GATT Client role and GATT Server role.
* You may use them in either roles.
*
* No letter is appended to the API name: Cy_BLE_CTS_
*
* \defgroup group_ble_service_api_CTS_server CTS Server Functions
* \ingroup group_ble_service_api_CTS
* \brief
* API unique to CTS designs configured as a GATT Server role.
*
* A letter 's' is appended to the API name: Cy_BLE_CTSS_
*
* \defgroup group_ble_service_api_CTS_client CTS Client Functions
* \ingroup group_ble_service_api_CTS
* \brief
* API unique to CTS designs configured as a GATT Client role.
*
* A letter 'c' is appended to the API name: Cy_BLE_CTSC_
*
* \defgroup group_ble_service_api_CTS_definitions CTS Definitions and Data Structures
* \ingroup group_ble_service_api_CTS
* \brief
* Contains the CTS specific definitions and data structures
* used in the CTS API.
*
*
* \defgroup group_ble_service_api_DIS Device Information Service (DIS)
* \ingroup group_ble_service_api
* \brief
* The Device Information Service exposes manufacturer and/or
* vendor information about a device.
*
* Depending on the chosen GATT role in the GUI, you may
* use a subset of the supported API.
*
* The DIS API names begin with Cy_BLE_DIS_. In addition to this, the API
* also append the GATT role initial letter in the API name.
*
* \defgroup group_ble_service_api_DIS_server_client DIS Server and Client Function
* \ingroup group_ble_service_api_DIS
* \brief
* These are API common to both GATT Client role and GATT Server role.
* You may use them in either roles.
*
* No letter is appended to the API name: Cy_BLE_DIS_
*
* \defgroup group_ble_service_api_DIS_server DIS Server Functions
* \ingroup group_ble_service_api_DIS
* \brief
* API unique to DIS designs configured as a GATT Server role.
*
* A letter 's' is appended to the API name: Cy_BLE_DISS_
*
* \defgroup group_ble_service_api_DIS_client DIS Client Functions
* \ingroup group_ble_service_api_DIS
* \brief
* API unique to DIS designs configured as a GATT Client role.
*
* A letter 'c' is appended to the API name: Cy_BLE_DISC_
*
* \defgroup group_ble_service_api_DIS_definitions DIS Definitions and Data Structures
* \ingroup group_ble_service_api_DIS
* \brief
* Contains the DIS specific definitions and data structures
* used in the DIS API.
*
*
*
* \defgroup group_ble_service_api_ESS Environmental Sensing Service (ESS)
* \ingroup group_ble_service_api
* \brief
* The Environmental Sensing Service exposes measurement data from an environmental sensor intended for sports and fitness
* applications.
*
* Depending on the chosen GATT role in the GUI, you may use a subset of the supported API.
*
* The ESS API names begin with Cy_BLE_ESS_. In addition to this, the API also append the GATT role initial letter in the
* API name.
*
* \defgroup group_ble_service_api_ESS_server_client ESS Server and Client Function
* \ingroup group_ble_service_api_ESS
* \brief
* These are API common to both GATT Client role and GATT Server role. You may use them in either roles.
*
* No letter is appended to the API name: Cy_BLE_ESS_
*
* \defgroup group_ble_service_api_ESS_server ESS Server Functions
* \ingroup group_ble_service_api_ESS
* \brief
* API unique to ESS designs configured as a GATT Server role.
*
* A letter 's' is appended to the API name: Cy_BLE_ESSS_
*
* \defgroup group_ble_service_api_ESS_client ESS Client Functions
* \ingroup group_ble_service_api_ESS
* \brief
* API unique to ESS designs configured as a GATT Client role.
*
* A letter 'c' is appended to the API name: Cy_BLE_ESSC_
*
* \defgroup group_ble_service_api_ESS_definitions ESS Definitions and Data Structures
* \ingroup group_ble_service_api_ESS
* \brief
* Contains the ESS specific definitions and data structures used in the ESS API.
*
*
*
* \defgroup group_ble_service_api_GLS Glucose Service (GLS)
* \ingroup group_ble_service_api
* \brief
* The Glucose Service exposes glucose and other data related to
* a personal glucose sensor for consumer healthcare applications
* and is not designed for clinical use.
*
* Depending on the chosen GATT role in the GUI, you may
* use a subset of the supported API.
*
* The GLS API names begin with Cy_BLE_GLS_. In addition to this, the API
* also append the GATT role initial letter in the API name.
*
* \defgroup group_ble_service_api_GLS_server_client GLS Server and Client Function
* \ingroup group_ble_service_api_GLS
* \brief
* These are API common to both GATT Client role and GATT Server role.
* You may use them in either roles.
*
* No letter is appended to the API name: Cy_BLE_GLS_
*
* \defgroup group_ble_service_api_GLS_server GLS Server Functions
* \ingroup group_ble_service_api_GLS
* \brief
* API unique to GLS designs configured as a GATT Server role.
*
* A letter 's' is appended to the API name: Cy_BLE_GLSS_
*
* \defgroup group_ble_service_api_GLS_client GLS Client Functions
* \ingroup group_ble_service_api_GLS
* \brief
* API unique to GLS designs configured as a GATT Client role.
*
* A letter 'c' is appended to the API name: Cy_BLE_GLSC_
*
* \defgroup group_ble_service_api_GLS_definitions GLS Definitions and Data Structures
* \ingroup group_ble_service_api_GLS
* \brief
* Contains the GLS specific definitions and data structures
* used in the GLS API.
*
*
*
* \defgroup group_ble_service_api_HIDS HID Service (HIDS)
* \ingroup group_ble_service_api
* \brief
* The HID Service exposes data and associated formatting for
* HID Devices and HID Hosts.
*
* Depending on the chosen GATT role in the GUI, you may
* use a subset of the supported API.
*
* The HID API names begin with Cy_BLE_HID. In addition to this, the API
* also append the GATT role initial letter in the API name.
*
* \defgroup group_ble_service_api_HIDS_server_client HIDS Server and Client Functions
* \ingroup group_ble_service_api_HIDS
* \brief
* These are API common to both GATT Client role and GATT Server role.
* You may use them in either roles.
*
* No letter is appended to the API name: Cy_BLE_HIDS_
*
* \defgroup group_ble_service_api_HIDS_server HIDS Server Functions
* \ingroup group_ble_service_api_HIDS
* \brief
* API unique to HID designs configured as a GATT Server role.
*
* A letter 's' is appended to the API name: Cy_BLE_HIDSS_
*
* \defgroup group_ble_service_api_HIDS_client HIDS Client Functions
* \ingroup group_ble_service_api_HIDS
* \brief
* API unique to HID designs configured as a GATT Client role.
*
* A letter 'c' is appended to the API name: Cy_BLE_HIDSC_
*
* \defgroup group_ble_service_api_HIDS_definitions HIDS Definitions and Data Structures
* \ingroup group_ble_service_api_HIDS
* \brief
* Contains the HID specific definitions and data structures
* used in the HID API.
*
*
*
* \defgroup group_ble_service_api_HRS Heart Rate Service (HRS)
* \ingroup group_ble_service_api
* \brief
* The Heart Rate Service exposes heart rate and other data
* related to a heart rate sensor intended for fitness applications.
*
* Depending on the chosen GATT role in the GUI, you may
* use a subset of the supported API.
*
* The HRS API names begin with Cy_BLE_HRS_. In addition to this, the API
* also append the GATT role initial letter in the API name.
*
* \defgroup group_ble_service_api_HRS_server_client HRS Server and Client Function
* \ingroup group_ble_service_api_HRS
* \brief
* These are API common to both GATT Client role and GATT Server role.
* You may use them in either roles.
*
* No letter is appended to the API name: Cy_BLE_HRS_
*
* \defgroup group_ble_service_api_HRS_server HRS Server Functions
* \ingroup group_ble_service_api_HRS
* \brief
* API unique to HRS designs configured as a GATT Server role.
*
* A letter 's' is appended to the API name: Cy_BLE_HRSS_
*
* \defgroup group_ble_service_api_HRS_client HRS Client Functions
* \ingroup group_ble_service_api_HRS
* \brief
* API unique to HRS designs configured as a GATT Client role.
*
* A letter 'c' is appended to the API name: Cy_BLE_HRSC_
*
* \defgroup group_ble_service_api_HRS_definitions HRS Definitions and Data Structures
* \ingroup group_ble_service_api_HRS
* \brief
* Contains the HRS specific definitions and data structures
* used in the HRS API.
*
*
*
* \defgroup group_ble_service_api_HPS HTTP Proxy Service (HPS)
* \ingroup group_ble_service_api
* \brief
* The HTTP Proxy Service allows a Client device, typically a sensor,
* to communicate with a Web Server through a gateway device.
*
* Depending on the chosen GATT role in the GUI, you may
* use a subset of the supported API.
*
* The HPS API names begin with Cy_BLE_HPS_. In addition to this, the API
* also append the GATT role initial letter in the API name.
*
* \defgroup group_ble_service_api_HPS_server_client HPS Server and Client Function
* \ingroup group_ble_service_api_HPS
* \brief
* These are API common to both GATT Client role and GATT Server role.
* You may use them in either roles.
*
* No letter is appended to the API name: Cy_BLE_HPS_
*
* \defgroup group_ble_service_api_HPS_server HPS Server Functions
* \ingroup group_ble_service_api_HPS
* \brief
* API unique to HPS designs configured as a GATT Server role.
*
* A letter 's' is appended to the API name: Cy_BLE_HPSS_
*
* \defgroup group_ble_service_api_HPS_client HPS Client Functions
* \ingroup group_ble_service_api_HPS
* \brief
* API unique to HPS designs configured as a GATT Client role.
*
* A letter 'c' is appended to the API name: Cy_BLE_HPSC_
*
* \defgroup group_ble_service_api_HPS_definitions HPS Definitions and Data Structures
* \ingroup group_ble_service_api_HPS
* \brief
* Contains the HPS specific definitions and data structures
* used in the HPS API.
*
*
*
* \defgroup group_ble_service_api_HTS Health Thermometer Service (HTS)
* \ingroup group_ble_service_api
* \brief
* The Health Thermometer Service exposes temperature and other
* data related to a thermometer used for healthcare applications.
*
* Depending on the chosen GATT role in the GUI, you may
* use a subset of the supported API.
*
* The HTS API names begin with Cy_BLE_HTS_. In addition to this, the API
* also append the GATT role initial letter in the API name.
*
* \defgroup group_ble_service_api_HTS_server_client HTS Server and Client Function
* \ingroup group_ble_service_api_HTS
* \brief
* These are API common to both GATT Client role and GATT Server role.
* You may use them in either roles.
*
* No letter is appended to the API name: Cy_BLE_HTS_
*
* \defgroup group_ble_service_api_HTS_server HTS Server Functions
* \ingroup group_ble_service_api_HTS
* \brief
* API unique to HTS designs configured as a GATT Server role.
*
* A letter 's' is appended to the API name: Cy_BLE_HTSS_
*
* \defgroup group_ble_service_api_HTS_client HTS Client Functions
* \ingroup group_ble_service_api_HTS
* \brief
* API unique to HTS designs configured as a GATT Client role.
*
* A letter 'c' is appended to the API name: Cy_BLE_HTSC_
*
* \defgroup group_ble_service_api_HTS_definitions HTS Definitions and Data Structures
* \ingroup group_ble_service_api_HTS
* \brief
* Contains the HTS specific definitions and data structures
* used in the HTS API.
*
*
*
* \defgroup group_ble_service_api_IAS Immediate Alert Service (IAS)
* \ingroup group_ble_service_api
* \brief
* The Immediate Alert Service exposes a control point to allow a peer device to cause the device to immediately alert.
*
* The Immediate Alert Service uses the Alert Level Characteristic
* to cause an alert when it is written with a value other than "No Alert".
*
* Depending on the chosen GATT role in the GUI, you may
* use a subset of the supported API.
*
* The IAS API names begin with Cy_BLE_IAS_. In addition to this, the API
* also append the GATT role initial letter in the API name.
*
* \defgroup group_ble_service_api_IAS_server_client IAS Server and Client Function
* \ingroup group_ble_service_api_IAS
* \brief
* These are API common to both GATT Client role and GATT Server role.
* You may use them in either roles.
*
* No letter is appended to the API name: Cy_BLE_IAS_
*
* \defgroup group_ble_service_api_IAS_server IAS Server Functions
* \ingroup group_ble_service_api_IAS
* \brief
* API unique to IAS designs configured as a GATT Server role.
*
* A letter 's' is appended to the API name: Cy_BLE_IASS_
*
* \defgroup group_ble_service_api_IAS_client IAS Client Functions
* \ingroup group_ble_service_api_IAS
* \brief
* API unique to IAS designs configured as a GATT Client role.
*
* A letter 'c' is appended to the API name: Cy_BLE_IASC_
*
* \defgroup group_ble_service_api_IAS_definitions IAS Definitions and Data Structures
* \ingroup group_ble_service_api_IAS
* \brief
* Contains the IAS specific definitions and data structures
* used in the IAS API.
*
*
*
* \defgroup group_ble_service_api_IPS Indoor Positioning Service (IPS)
* \ingroup group_ble_service_api
* \brief
* The Indoor Positioning exposes coordinates and other location related information via
* an advertisement or indicates that the device address can be used for location look-up,
* enabling mobile devices to find their position.
*
* Depending on the chosen GATT role in the GUI, you may
* use a subset of the supported API.
*
* The IPS API names begin with Cy_BLE_IPS_. In addition to this, the API
* also append the GATT role initial letter in the API name.
*
* \defgroup group_ble_service_api_IPS_server_client IPS Server and Client Function
* \ingroup group_ble_service_api_IPS
* \brief
* These are API common to both GATT Client role and GATT Server role.
* You may use them in either roles.
*
* No letter is appended to the API name: Cy_BLE_IPS_
*
* \defgroup group_ble_service_api_IPS_server IPS Server Functions
* \ingroup group_ble_service_api_IPS
* \brief
* API unique to IPS designs configured as a GATT Server role.
*
* A letter 's' is appended to the API name: Cy_BLE_IPSS_
*
* \defgroup group_ble_service_api_IPS_client IPS Client Functions
* \ingroup group_ble_service_api_IPS
* \brief
* API unique to IPS designs configured as a GATT Client role.
*
* A letter 'c' is appended to the API name: Cy_BLE_IPSC_
*
* \defgroup group_ble_service_api_IPS_definitions IPS Definitions and Data Structures
* \ingroup group_ble_service_api_IPS
* \brief
* Contains the IPS specific definitions and data structures
* used in the IPS API.
*
*
*
* \defgroup group_ble_service_api_LLS Link Loss Service (LLS)
* \ingroup group_ble_service_api
* \brief
* The Link Loss Service uses the Alert Level Characteristic to
* cause an alert in the device when the link is lost.
*
* Depending on the chosen GATT role in the GUI, you may
* use a subset of the supported API.
*
* The LLS API names begin with Cy_BLE_LLS_. In addition to this, the API
* also append the GATT role initial letter in the API name.
*
* \defgroup group_ble_service_api_LLS_server_client LLS Server and Client Function
* \ingroup group_ble_service_api_LLS
* \brief
* These are API common to both GATT Client role and GATT Server role.
* You may use them in either roles.
*
* No letter is appended to the API name: Cy_BLE_LLS_
*
* \defgroup group_ble_service_api_LLS_server LLS Server Functions
* \ingroup group_ble_service_api_LLS
* \brief
* API unique to LLS designs configured as a GATT Server role.
*
* A letter 's' is appended to the API name: Cy_BLE_LLSS_
*
* \defgroup group_ble_service_api_LLS_client LLS Client Functions
* \ingroup group_ble_service_api_LLS
* \brief
* API unique to LLS designs configured as a GATT Client role.
*
* A letter 'c' is appended to the API name: Cy_BLE_LLSC_
*
* \defgroup group_ble_service_api_LLS_definitions LLS Definitions and Data Structures
* \ingroup group_ble_service_api_LLS
* \brief
* Contains the LLS specific definitions and data structures
* used in the LLS API.
*
*
*
* \defgroup group_ble_service_api_LNS Location and Navigation Service (LNS)
* \ingroup group_ble_service_api
* \brief
* The Location and Navigation Service exposes location and
* navigation-related data from a Location and Navigation sensor
* (Server) intended for outdoor activity applications.
*
* Depending on the chosen GATT role in the GUI, you may
* use a subset of the supported API.
*
* The LNS API names begin with Cy_BLE_LNS_. In addition to this, the API
* also append the GATT role initial letter in the API name.
*
* \defgroup group_ble_service_api_LNS_server_client LNS Server and Client Function
* \ingroup group_ble_service_api_LNS
* \brief
* These are API common to both GATT Client role and GATT Server role.
* You may use them in either roles.
*
* No letter is appended to the API name: Cy_BLE_LNS_
*
* \defgroup group_ble_service_api_LNS_server LNS Server Functions
* \ingroup group_ble_service_api_LNS
* \brief
* API unique to LNS designs configured as a GATT Server role.
*
* A letter 's' is appended to the API name: Cy_BLE_LNSS_
*
* \defgroup group_ble_service_api_LNS_client LNS Client Functions
* \ingroup group_ble_service_api_LNS
* \brief
* API unique to LNS designs configured as a GATT Client role.
*
* A letter 'c' is appended to the API name: Cy_BLE_LNSC_
*
* \defgroup group_ble_service_api_LNS_definitions LNS Definitions and Data Structures
* \ingroup group_ble_service_api_LNS
* \brief
* Contains the LNS specific definitions and data structures
* used in the LNS API.
*
*
*
* \defgroup group_ble_service_api_NDCS Next DST Change Service (NDCS)
* \ingroup group_ble_service_api
* \brief
* The Next DST Change Service enables a BLE device that has knowledge about the
* next occurrence of a DST change to expose this information to
* another Bluetooth device. The Service uses the "Time with DST"
* Characteristic and the functions exposed in this Service are
* used to interact with that Characteristic.
*
* Depending on the chosen GATT role in the GUI, you may
* use a subset of the supported API.
*
* The NDCS API names begin with Cy_BLE_NDCS_. In addition to this, the API
* also append the GATT role initial letter in the API name.
*
* \defgroup group_ble_service_api_NDCS_server_client NDCS Server and Client Functions
* \ingroup group_ble_service_api_NDCS
* \brief
* These are API common to both GATT Client role and GATT Server role.
* You may use them in either roles.
*
* No letter is appended to the API name: Cy_BLE_NDCS_
*
* \defgroup group_ble_service_api_NDCS_server NDCS Server Functions
* \ingroup group_ble_service_api_NDCS
* \brief
* API unique to NDCS designs configured as a GATT Server role.
*
* A letter 's' is appended to the API name: Cy_BLE_NDCSS_
*
* \defgroup group_ble_service_api_NDCS_client NDCS Client Functions
* \ingroup group_ble_service_api_NDCS
* \brief
* API unique to NDCS designs configured as a GATT Client role.
*
* A letter 'c' is appended to the API name: Cy_BLE_NDCSC_
*
* \defgroup group_ble_service_api_NDCS_definitions NDCS Definitions and Data Structures
* \ingroup group_ble_service_api_NDCS
* \brief
* Contains the NDCS specific definitions and data structures
* used in the NDCS API.
*
*
*
* \defgroup group_ble_service_api_PASS Phone Alert Status Service (PASS)
* \ingroup group_ble_service_api
* \brief
* The Phone Alert Status Service uses the Alert Status Characteristic
* and Ringer Setting Characteristic to expose the phone alert status
* and uses the Ringer Control Point Characteristic to control the
* phone's ringer into mute or enable.
*
* Depending on the chosen GATT role in the GUI, you may
* use a subset of the supported API.
*
* The PASS API names begin with Cy_BLE_PASS_. In addition to this, the API
* also append the GATT role initial letter in the API name.
*
* \defgroup group_ble_service_api_PASS_server_client PASS Server and Client Function
* \ingroup group_ble_service_api_PASS
* \brief
* These are API common to both GATT Client role and GATT Server role.
* You may use them in either roles.
*
* No letter is appended to the API name: Cy_BLE_PASS_
*
* \defgroup group_ble_service_api_PASS_server PASS Server Functions
* \ingroup group_ble_service_api_PASS
* \brief
* API unique to PASS designs configured as a GATT Server role.
*
* A letter 's' is appended to the API name: Cy_BLE_PASSS_
*
* \defgroup group_ble_service_api_PASS_client PASS Client Functions
* \ingroup group_ble_service_api_PASS
* \brief
* API unique to PASS designs configured as a GATT Client role.
*
* A letter 'c' is appended to the API name: Cy_BLE_PASSC_
*
* \defgroup group_ble_service_api_PASS_definitions PASS Definitions and Data Structures
* \ingroup group_ble_service_api_PASS
* \brief
* Contains the PASS specific definitions and data structures
* used in the PASS API.
*
*
*
* \defgroup group_ble_service_api_PLXS Pulse Oximeter Service (PLXS)
* \ingroup group_ble_service_api
* \brief
* The Pulse Oximeter Service enables a Collector device to connect and interact with a pulse oximeter intended for healthcare applications.
*
* Depending on the chosen GATT role in the GUI, you may
* use a subset of the supported API.
*
* The PLXS API names begin with Cy_BLE_PLXS_. In addition to this, the API
* also append the GATT role initial letter in the API name.
*
* \defgroup group_ble_service_api_PLXS_server_client PLXS Server and Client Function
* \ingroup group_ble_service_api_PLXS
* \brief
* These are API common to both GATT Client role and GATT Server role.
* You may use them in either roles.
*
* No letter is appended to the API name: Cy_BLE_PLXS_
*
* \defgroup group_ble_service_api_PLXS_server PLXS Server Functions
* \ingroup group_ble_service_api_PLXS
* \brief
* API unique to PLXS designs configured as a GATT Server role.
*
* A letter 's' is appended to the API name: Cy_BLE_PLXSS_
*
* \defgroup group_ble_service_api_PLXS_client PLXS Client Functions
* \ingroup group_ble_service_api_PLXS
* \brief
* API unique to PLXS designs configured as a GATT Client role.
*
* A letter 'c' is appended to the API name: Cy_BLE_PLXSC_
*
* \defgroup group_ble_service_api_PLXS_definitions PLXS Definitions and Data Structures
* \ingroup group_ble_service_api_PLXS
* \brief
* Contains the PLXS specific definitions and data structures used
* in the PLXS API.
*
*
*
* \defgroup group_ble_service_api_RSCS Running Speed and Cadence Service (RSCS)
* \ingroup group_ble_service_api
* \brief
* The Running Speed and Cadence (RSC) Service exposes speed,
* cadence and other data related to fitness applications such
* as the stride length and the total distance the user has
* travelled while using the Running Speed and Cadence sensor (Server).
*
* Depending on the chosen GATT role in the GUI, you may
* use a subset of the supported API.
*
* The RSCS API names begin with Cy_BLE_RSCS_. In addition to this, the API
* also append the GATT role initial letter in the API name.
*
* \defgroup group_ble_service_api_RSCS_server_client RSCS Server and Client Functions
* \ingroup group_ble_service_api_RSCS
* \brief
* These are API common to both GATT Client role and GATT Server role.
* You may use them in either roles.
*
* No letter is appended to the API name: Cy_BLE_RSCS_
*
* \defgroup group_ble_service_api_RSCS_server RSCS Server Functions
* \ingroup group_ble_service_api_RSCS
* \brief
* API unique to RSCS designs configured as a GATT Server role.
*
* A letter 's' is appended to the API name: Cy_BLE_RSCSS_
*
* \defgroup group_ble_service_api_RSCS_client RSCS Client Functions
* \ingroup group_ble_service_api_RSCS
* \brief
* API unique to RSCS designs configured as a GATT Client role.
*
* A letter 'c' is appended to the API name: Cy_BLE_RSCSC_
*
* \defgroup group_ble_service_api_RSCS_definitions RSCS Definitions and Data Structures
* \ingroup group_ble_service_api_RSCS
* \brief
* Contains the RSCS specific definitions and data structures
* used in the RSCS API.
*
*
*
* \defgroup group_ble_service_api_RTUS Reference Time Update Service (RTUS)
* \ingroup group_ble_service_api
* \brief
* The Reference Time Update Service enables a Bluetooth device that can update the
* system time using the reference time such as a GPS receiver
* to expose a control point and expose the accuracy (drift) of
* the local system time compared to the reference time source.
*
* Depending on the chosen GATT role in the GUI, you may
* use a subset of the supported API.
*
* The RTUS API names begin with Cy_BLE_RTUS_. In addition to this, the API
* also append the GATT role initial letter in the API name.
*
* \defgroup group_ble_service_api_RTUS_server_client RTUS Server and Client Function
* \ingroup group_ble_service_api_RTUS
* \brief
* These are API common to both GATT Client role and GATT Server role.
* You may use them in either roles.
*
* No letter is appended to the API name: Cy_BLE_RTUS_
*
* \defgroup group_ble_service_api_RTUS_server RTUS Server Functions
* \ingroup group_ble_service_api_RTUS
* \brief
* API unique to RTUS designs configured as a GATT Server role.
*
* A letter 's' is appended to the API name: Cy_BLE_RTUSS_
*
* \defgroup group_ble_service_api_RTUS_client RTUS Client Functions
* \ingroup group_ble_service_api_RTUS
* \brief
* API unique to RTUS designs configured as a GATT Client role.
*
* A letter 'c' is appended to the API name: Cy_BLE_RTUSC_
*
* \defgroup group_ble_service_api_RTUS_definitions RTUS Definitions and Data Structures
* \ingroup group_ble_service_api_RTUS
* \brief
* Contains the RTUS specific definitions and data structures
* used in the RTUS API.
*
*
*
* \defgroup group_ble_service_api_SCPS Scan Parameters Service (ScPS)
* \ingroup group_ble_service_api
* \brief
* The Scan Parameters Service enables a Server device to expose
* a Characteristic for the GATT Client to write its scan interval
* and scan window on the Server device, and enables a Server to
* request a refresh of the GATT Client scan interval and scan window.
*
* Depending on the chosen GATT role in the GUI, you may
* use a subset of the supported API.
*
* The ScPS API names begin with Cy_BLE_SCPS_. In addition to this, the API
* also append the GATT role initial letter in the API name.
*
* \defgroup group_ble_service_api_SCPS_server_client ScPS Server and Client Functions
* \ingroup group_ble_service_api_SCPS
* \brief
* These are API common to both GATT Client role and GATT Server role.
* You may use them in either roles.
*
* No letter is appended to the API name: Cy_BLE_SCPS_
*
* \defgroup group_ble_service_api_SCPS_server ScPS Server Functions
* \ingroup group_ble_service_api_SCPS
* \brief
* API unique to ScPS designs configured as a GATT Server role.
*
* A letter 's' is appended to the API name: Cy_BLE_SCPSS_
*
* \defgroup group_ble_service_api_SCPS_client ScPS Client Functions
* \ingroup group_ble_service_api_SCPS
* \brief
* API unique to ScPS designs configured as a GATT Client role.
*
* A letter 'c' is appended to the API name: Cy_BLE_SCPSC_
*
* \defgroup group_ble_service_api_SCPS_definitions ScPS Definitions and Data Structures
* \ingroup group_ble_service_api_SCPS
* \brief
* Contains the ScPS specific definitions and data structures
* used in the ScPS API.
*
*
*
* \defgroup group_ble_service_api_TPS TX Power Service (TPS)
* \ingroup group_ble_service_api
* \brief
* The Tx Power Service uses the Tx Power Level Characteristic
* to expose the current transmit power level of a device when
* in a connection.
*
* Depending on the chosen GATT role in the GUI, you may
* use a subset of the supported API.
*
* The TPS API names begin with Cy_BLE_TPS_. In addition to this, the API
* also append the GATT role initial letter in the API name.
*
* \defgroup group_ble_service_api_TPS_server_client TPS Server and Client Function
* \ingroup group_ble_service_api_TPS
* \brief
* These are API common to both GATT Client role and GATT Server role.
* You may use them in either roles.
*
* No letter is appended to the API name: Cy_BLE_TPS_
*
* \defgroup group_ble_service_api_TPS_server TPS Server Functions
* \ingroup group_ble_service_api_TPS
* \brief
* API unique to TPS designs configured as a GATT Server role.
*
* A letter 's' is appended to the API name: Cy_BLE_TPSS_
*
* \defgroup group_ble_service_api_TPS_client TPS Client Functions
* \ingroup group_ble_service_api_TPS
* \brief
* API unique to TPS designs configured as a GATT Client role.
*
* A letter 'c' is appended to the API name: Cy_BLE_TPSC_
*
* \defgroup group_ble_service_api_TPS_definitions TPS Definitions and Data Structures
* \ingroup group_ble_service_api_TPS
* \brief
* Contains the TPS specific definitions and data structures
* used in the TPS API.
*
*
*
* \defgroup group_ble_service_api_UDS User Data Service (UDS)
* \ingroup group_ble_service_api
* \brief
* The User Data Service exposes user-related data in the sports and fitness environment. This allows remote access and
* update of user data by a Client as well as the synchronization of user data between a Server and a Client.
*
* Depending on the chosen GATT role in the GUI, you may use a subset of the supported API.
*
* The UDS API names begin with Cy_BLE_UDS_. In addition to this, the API also append the GATT role initial letter in the
* API name.
*
* \defgroup group_ble_service_api_UDS_server_client UDS Server and Client Function
* \ingroup group_ble_service_api_UDS
* \brief
* These are API common to both GATT Client role and GATT Server role. You may use them in either roles.
*
* No letter is appended to the API name: Cy_BLE_UDS_
*
* \defgroup group_ble_service_api_UDS_server UDS Server Functions
* \ingroup group_ble_service_api_UDS
* \brief
* API unique to UDS designs configured as a GATT Server role.
*
* A letter 's' is appended to the API name: Cy_BLE_UDSS_
*
* \defgroup group_ble_service_api_UDS_client UDS Client Functions
* \ingroup group_ble_service_api_UDS
* \brief
* API unique to UDS designs configured as a GATT Client role.
*
* A letter 'c' is appended to the API name: Cy_BLE_UDSC_
*
* \defgroup group_ble_service_api_UDS_definitions UDS Definitions and Data Structures
* \ingroup group_ble_service_api_UDS
* \brief
* Contains the UDS specific definitions and data structures used in the UDS API.
*
*
*
*
* \defgroup group_ble_service_api_WPTS Wireless Power Transfer Service (WPTS)
* \ingroup group_ble_service_api
* \brief
* The Wireless Power Transfer Service enables communication between Power Receiver Unit and Power Transmitter Unit
* in the Wireless Power Transfer systems.
*
* Depending on the chosen GATT role in the GUI, you may use a subset of the supported API.
*
* The WPTS API names begin with Cy_BLE_WPTS_. In addition to this, the API also append the GATT role initial letter in the
* API name.
*
* \defgroup group_ble_service_api_WPTS_server_client WPTS Server and Client Function
* \ingroup group_ble_service_api_WPTS
* \brief
* These are API common to both GATT Client role and GATT Server role. You may use them in either roles.
*
* No letter is appended to the API name: Cy_BLE_WPTS_
*
* \defgroup group_ble_service_api_WPTS_server WPTS Server Functions
* \ingroup group_ble_service_api_WPTS
* \brief
* API unique to WPTS designs configured as a GATT Server role.
*
* A letter 's' is appended to the API name: Cy_BLE_WPTSS_
*
* \defgroup group_ble_service_api_WPTS_client WPTS Client Functions
* \ingroup group_ble_service_api_WPTS
* \brief
* API unique to WPTS designs configured as a GATT Client role.
*
* A letter 'c' is appended to the API name: Cy_BLE_WPTSC_
*
* \defgroup group_ble_service_api_WPTS_definitions WPTS Definitions and Data Structures
* \ingroup group_ble_service_api_WPTS
* \brief
* Contains the WPTS specific definitions and data structures used in the WPTS API.
*
*
*
* \defgroup group_ble_service_api_WSS Weight Scale Service (WSS)
* \ingroup group_ble_service_api
* \brief
* The Weight Scale Service exposes weight and related data from a weight scale (Server) intended for consumer healthcare
* as well as sports/fitness applications.
*
* Depending on the chosen GATT role in the GUI, you may use a subset of the supported API.
*
* The WSS API names begin with Cy_BLE_WSS_. In addition to this, the API also append the GATT role initial letter in the
* API name.
*
* \defgroup group_ble_service_api_WSS_server_client WSS Server and Client Function
* \ingroup group_ble_service_api_WSS
* \brief
* These are API common to both GATT Client role and GATT Server role. You may use them in either roles.
*
* No letter is appended to the API name: Cy_BLE_WSS_
*
* \defgroup group_ble_service_api_WSS_server WSS Server Functions
* \ingroup group_ble_service_api_WSS
* \brief
* API unique to WSS designs configured as a GATT Server role.
*
* A letter 's' is appended to the API name: Cy_BLE_WSSS_
*
* \defgroup group_ble_service_api_WSS_client WSS Client Functions
* \ingroup group_ble_service_api_WSS
* \brief
* API unique to WSS designs configured as a GATT Client role.
*
* A letter 'c' is appended to the API name: Cy_BLE_WSSC_
*
* \defgroup group_ble_service_api_WSS_definitions WSS Definitions and Data Structures
* \ingroup group_ble_service_api_WSS
* \brief
* Contains the WSS specific definitions and data structures used in the WSS API.
*
*
* \defgroup group_ble_service_api_custom Custom Service
* \ingroup group_ble_service_api
* \brief
* This section contains the the data structures used for Custom Services.
*
*
*
* \defgroup group_ble_clock_api_section BLE Clock API
* \ingroup group_ble
* \brief
*  This section describes the BLE ECO clock.
*
*
* \defgroup group_ble_clock_api Functions
* \ingroup group_ble_clock_api_section
*
* \defgroup group_ble_clock_api_data_types  Definitions and Data Structures
* \ingroup group_ble_clock_api_section
*
*/


 
 /**
 * \addtogroup group_ble_common_api_creator_functions
 * @{
 */
/******************************************************************************
* Function Name: Cy_BLE_Start
***************************************************************************//**
* 
*  This function initializes the BLE Stack which consists of the BLE Stack 
*  Manager, BLE Controller, and BLE Host modules. This function takes care of
*  initializing the Profile layer, schedulers, Timer and other platform-related
*  resources required for the BLE component. It also registers the callback 
*  function for BLE events that will be registered in the BLE stack.
* 
*  Note This function does not reset the BLE Stack.
* 
*  For HCI mode of operation, this function will not initialize the BLE Host 
*  module.
* 
*  Calling this function results in generating a \ref  CY_BLE_EVT_STACK_ON event
*  on successful initialization of the BLE Stack.
*
*  In Dual Core mode, this function is called on both cores in the following 
*  sequence:
*   - Call this function on the CM0+ core to initialize the Controller.
*   - Start the CM4 core by calling the Cy_SysEnableCM4() function.
*   - Call this function on the CM4 core to initialize the Host and Profiles.
* 
*  BLE Stack enables the BLE ECO clock automatically with the default
*  parameters:
*     Parameter            | Value
*     -------------------- | -----------
*     ECO Frequency        | CY_BLE_DEFAULT_ECO_FREQ
*     Divider              | CY_BLE_DEFAULT_ECO_DIV
*     Startup time         | CY_BLE_DEFAULT_OSC_STARTUP_DELAY_LF
*     Load cap             | CY_BLE_DEFAULT_CAP_TRIM_VALUE
*  If there is a need to start the BLE with non-default ECO parameters, 
*  call the Cy_BLE_Start() function with the custom configuration each 
*  time before calling the Cy_BLE_Enable() function.
*
*  \param callbackFunc: The event callback function to receive events from the 
*   BLE stack. cy_ble_callback_t is a function pointer type.
* 
* \return
*  \ref cy_en_ble_api_result_t : The return value indicates if the function succeeded or 
*  failed. Possible error codes:
* 
*   <table>
*        <tr>
*       <th>Error codes</th>
*       <th>Description</th>
*     </tr>
*     <tr>
*       <td>CY_BLE_SUCCESS</td>
*       <td>On successful operation.</td>
*     </tr>
*     <tr>
*       <td>CY_BLE_ERROR_INVALID_PARAMETER</td>
*       <td>On passing a NULL pointer to the function when the BLE stack is not built in HCI mode. 
*            \ref CY_BLE_ERROR_INVALID_PARAMETER is never returned in HCI mode.</td>
*     </tr>
*     <tr>
*       <td>CY_BLE_ERROR_REPEATED_ATTEMPTS</td>
*       <td>On invoking this function more than once without calling the Cy_BLE_StackShutdown() 
*           function between calls to this function.</td>
*     </tr>
*     <tr>
*       <td>CY_BLE_ERROR_MEMORY_ALLOCATION_FAILED</td>
*       <td>There is insufficient memory available.</td>
*     </tr>
*   </table>
* 
******************************************************************************/
cy_en_ble_api_result_t Cy_BLE_Start(cy_ble_callback_t callbackFunc)
{   
    /**
     * This function is implemented in the BLE Component of PSoC Creator 
     * Refer to BLE.c for the generated code.
     */
    
    return (CY_BLE_SUCCESS);
}


/******************************************************************************
* Function Name: Cy_BLE_Stop
***************************************************************************//**
* 
*  This function stops any ongoing operation in the BLE Stack and forces the 
*  BLE Stack to shut down. The only function that can be called after calling
*  this function is Cy_BLE_Start() when the Cy_BLE_GetState() API returns the
*  \ref CY_BLE_STATE_STOPPED state. 
*
*  Calling this function results in generating of 
*  a \ref CY_BLE_EVT_STACK_SHUTDOWN_COMPLETE event on a successful stack shutdown.
* 
* \return
*  \ref cy_en_ble_api_result_t : The return value indicates if the function succeeded or
*  failed. Possible error codes:
*
*    Errors codes                    | Description
*    ------------                    | -----------
*    CY_BLE_SUCCESS                  | On successful operation.
*    CY_BLE_ERROR_INVALID_OPERATION  | On calling Cy_BLE_Stop() before calling Cy_BLE_Start().
*
******************************************************************************/
cy_en_ble_api_result_t Cy_BLE_Stop(void)
{
    /**
     * This function is implemented in the BLE Component of PSoC Creator 
     * Refer to BLE.c for the generated code.
     */
    return(CY_BLE_SUCCESS);
}

/** @} */
