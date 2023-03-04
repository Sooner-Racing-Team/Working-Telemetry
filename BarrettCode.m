classdef BarrettCode < matlab.apps.AppBase

    % Properties that correspond to app components
    properties (Access = public)
        UIFigure           matlab.ui.Figure
        GridLayout         matlab.ui.container.GridLayout
        LeftPanel          matlab.ui.container.Panel
        EditField          matlab.ui.control.EditField
        DurationLabel      matlab.ui.control.Label
        COMEditField       matlab.ui.control.EditField
        COMLabel           matlab.ui.control.Label
        RunEditField       matlab.ui.control.EditField
        RunEditFieldLabel  matlab.ui.control.Label
        DriverEditField    matlab.ui.control.EditField
        DriverLabel        matlab.ui.control.Label
        Lamp               matlab.ui.control.Lamp
        STOPButton         matlab.ui.control.StateButton
        STARTButton        matlab.ui.control.Button
        RightPanel         matlab.ui.container.Panel
        TabGroup           matlab.ui.container.TabGroup
        PowertrainTab      matlab.ui.container.Tab
        UIAxes2_6          matlab.ui.control.UIAxes
        UIAxes2_5          matlab.ui.control.UIAxes
        UIAxes2_4          matlab.ui.control.UIAxes
        UIAxes2_3          matlab.ui.control.UIAxes
        UIAxes2_2          matlab.ui.control.UIAxes
        UIAxes2            matlab.ui.control.UIAxes
        ChassisTab         matlab.ui.container.Tab
        AeroTab            matlab.ui.container.Tab
        FLabel             matlab.ui.control.Label
    end

    % Properties that correspond to apps with auto-reflow
    properties (Access = private)
        onePanelWidth = 576;
    end

    % Callbacks that handle component events
    methods (Access = private)

        % Button pushed function: STARTButton
        function STARTButtonPushed(app, event)
            %Disconnects everything from the serial port so there are no
            %errors
            delete(instrfindall);
            
            %Clears the plots everytime the data restarts logging
            cla(app.UIAxes2_6);
            cla(app.UIAxes2_5);
            cla(app.UIAxes2_4);
            cla(app.UIAxes2_3);
            cla(app.UIAxes2_2);
            cla(app.UIAxes2);

            
            %Turns the lamp to green
            app.Lamp.Color = [0 1 0];

            %Creates an arduino object that connects to serial
            %arduino=serial('COM4','BaudRate',9600);

            arduino = serialport("COM4",9600);
            arduino.ReadAsyncMode = 'manual';


            %Opens the connections to the serial port
            %fopen(arduino)


            %DEFINITIONS
            timer = 0;
            max = 0;
            i = 1;

            savedArray = [0,0,0,0,0,0];

            %Animated line object intiatiation
            h1 = animatedline(app.UIAxes2_6,'Color','g','LineWidth',3);
            h2 = animatedline(app.UIAxes2_5,'Color','g','LineWidth',3);
            h3 = animatedline(app.UIAxes2_4,'Color','g','LineWidth',3);
            h4 = animatedline(app.UIAxes2_3,'Color','g','LineWidth',3);
            h5 = animatedline(app.UIAxes2_2,'Color','g','LineWidth',3);
            h6 = animatedline(app.UIAxes2,'Color','g','LineWidth',3);

                %Updates the limits of the plots

            ylim(app.UIAxes2_6,[-4.0,4.0]);

            ylim(app.UIAxes2_5,[-4.0,4.0]);

            ylim(app.UIAxes2_4,[50.0,100.0]);

            ylim(app.UIAxes2_3,[50.0,100.0]);

            ylim(app.UIAxes2_2,[50.0,100.0]);

            ylim(app.UIAxes2,[50.0,100.0]);
            

            %MAIN LOOP
            while 1
                %Reading serial data
                data = readline(arduino);
                

                
                if 
                

                end

                %Parses the CSV data from serial and outputs it to a cell
                cellData = textscan(data,'%f','Delimiter',',');

                %Extracts the data from a cell and puts it into an array
                %dataArray() = cellData{1}
                dataArray = cell2mat(cellData);

                %Checks to see if the arrays are empty and intializes them
                %with zero if they are.
                if isempty(dataArray)
                    dataArray(1) = 0;
                    dataArray(2) = 0;
                    dataArray(3) = 0;
                    dataArray(4) = 0;
                    dataArray(5) = 0;
                    dataArray(6) = 0;
                    %Add additional dataArray(x) for additional plots.
                end
                
                %Saving data to an array and excel sheet
                %savedArray(i,1) = dataArray(1);
                %savedArray(i,2) = dataArray(2);
                %savedArray(i,3) = dataArray(3);

                tempArray = [dataArray(1),dataArray(2),dataArray(3),dataArray(4),dataArray(5),dataArray(6)];
                savedArray = cat(1, savedArray, tempArray);
               % i = i + 1;

                %Adds the data points to each plot
                addpoints(h1,timer,dataArray(6));
                addpoints(h2,timer,dataArray(5)); 
                addpoints(h3,timer,dataArray(4)); 
                addpoints(h4,timer,dataArray(3));
                addpoints(h5,timer,dataArray(2)); 
                addpoints(h6,timer,dataArray(1)); 

                drawnow limitrate

                %Updates the limits of the plots
                xlim(app.UIAxes2_6,[-20+timer,5+timer]);

                xlim(app.UIAxes2_5,[-20+timer,5+timer]);

                xlim(app.UIAxes2_4,[-20+timer,5+timer]);

                xlim(app.UIAxes2_3,[-20+timer,5+timer]);

                xlim(app.UIAxes2_2,[-20+timer,5+timer]);

                xlim(app.UIAxes2,[-20+timer,5+timer]);


                
                %Progresses the timer/X-axis
                timer = timer + 0.2;


                %NOT WORKING NOT WORKING NOT WORKING
                %Sending data to the arduino
                %if app.SendButton.Value == 1
                %    configureTerminator(arduino,"CR");
                %    sender = app.ArduinoEditField.Value;
                %    stringSender = string(sender);
                %    %fprintf(arduino,sender); % send answer variable content to arduino
                %    writeline(arduino,stringSender);
                %    app.SendButton.Value = 0;
                %    app.Lamp.Color = [1 0 1];
                %end


                %STOP BUTTON 
                stopCommand = app.STOPButton.Value;
                if stopCommand == 1
                    app.STOPButton.Value = 0;
                    app.Lamp.Color = [1 0 0];
                    %cd colin\Downloads\
                    save('testArraySave.mat', 'savedArray', '-mat');
                    break
                end

                %Loop delay matches arduino's
                pause(0.001)
            end
            fclose(arduino);

        end

        % Changes arrangement of the app based on UIFigure width
        function updateAppLayout(app, event)
            currentFigureWidth = app.UIFigure.Position(3);
            if(currentFigureWidth <= app.onePanelWidth)
                % Change to a 2x1 grid
                app.GridLayout.RowHeight = {742, 742};
                app.GridLayout.ColumnWidth = {'1x'};
                app.RightPanel.Layout.Row = 2;
                app.RightPanel.Layout.Column = 1;
            else
                % Change to a 1x2 grid
                app.GridLayout.RowHeight = {'1x'};
                app.GridLayout.ColumnWidth = {211, '1x'};
                app.RightPanel.Layout.Row = 1;
                app.RightPanel.Layout.Column = 2;
            end
        end
    end

    % Component initialization
    methods (Access = private)

        % Create UIFigure and components
        function createComponents(app)

            % Create UIFigure and hide until all components are created
            app.UIFigure = uifigure('Visible', 'off');
            app.UIFigure.AutoResizeChildren = 'off';
            app.UIFigure.Position = [100 100 953 742];
            app.UIFigure.Name = 'MATLAB App';
            app.UIFigure.SizeChangedFcn = createCallbackFcn(app, @updateAppLayout, true);

            % Create GridLayout
            app.GridLayout = uigridlayout(app.UIFigure);
            app.GridLayout.ColumnWidth = {211, '1x'};
            app.GridLayout.RowHeight = {'1x'};
            app.GridLayout.ColumnSpacing = 0;
            app.GridLayout.RowSpacing = 0;
            app.GridLayout.Padding = [0 0 0 0];
            app.GridLayout.Scrollable = 'on';

            % Create LeftPanel
            app.LeftPanel = uipanel(app.GridLayout);
            app.LeftPanel.BackgroundColor = [0.8 0.8 0.8];
            app.LeftPanel.Layout.Row = 1;
            app.LeftPanel.Layout.Column = 1;

            % Create STARTButton
            app.STARTButton = uibutton(app.LeftPanel, 'push');
            app.STARTButton.ButtonPushedFcn = createCallbackFcn(app, @STARTButtonPushed, true);
            app.STARTButton.Position = [14 13 85 22];
            app.STARTButton.Text = 'START';

            % Create STOPButton
            app.STOPButton = uibutton(app.LeftPanel, 'state');
            app.STOPButton.Text = 'STOP';
            app.STOPButton.Position = [112 13 84 22];

            % Create Lamp
            app.Lamp = uilamp(app.LeftPanel);
            app.Lamp.Position = [7 717 20 20];
            app.Lamp.Color = [1 1 0.0667];

            % Create DriverLabel
            app.DriverLabel = uilabel(app.LeftPanel);
            app.DriverLabel.HorizontalAlignment = 'right';
            app.DriverLabel.FontSize = 15;
            app.DriverLabel.FontWeight = 'bold';
            app.DriverLabel.Position = [21 574 54 22];
            app.DriverLabel.Text = 'Driver:';

            % Create DriverEditField
            app.DriverEditField = uieditfield(app.LeftPanel, 'text');
            app.DriverEditField.Position = [90 574 100 22];

            % Create RunEditFieldLabel
            app.RunEditFieldLabel = uilabel(app.LeftPanel);
            app.RunEditFieldLabel.HorizontalAlignment = 'right';
            app.RunEditFieldLabel.FontSize = 15;
            app.RunEditFieldLabel.FontWeight = 'bold';
            app.RunEditFieldLabel.Position = [23 535 52 22];
            app.RunEditFieldLabel.Text = 'Run #:';

            % Create RunEditField
            app.RunEditField = uieditfield(app.LeftPanel, 'text');
            app.RunEditField.Position = [90 534 100 22];

            % Create COMLabel
            app.COMLabel = uilabel(app.LeftPanel);
            app.COMLabel.HorizontalAlignment = 'right';
            app.COMLabel.FontSize = 15;
            app.COMLabel.FontWeight = 'bold';
            app.COMLabel.Position = [30 496 45 22];
            app.COMLabel.Text = 'COM:';

            % Create COMEditField
            app.COMEditField = uieditfield(app.LeftPanel, 'text');
            app.COMEditField.Position = [90 496 100 22];

            % Create DurationLabel
            app.DurationLabel = uilabel(app.LeftPanel);
            app.DurationLabel.HorizontalAlignment = 'center';
            app.DurationLabel.FontSize = 18;
            app.DurationLabel.FontWeight = 'bold';
            app.DurationLabel.Position = [70 672 79 22];
            app.DurationLabel.Text = 'Duration';

            % Create EditField
            app.EditField = uieditfield(app.LeftPanel, 'text');
            app.EditField.HorizontalAlignment = 'center';
            app.EditField.FontSize = 24;
            app.EditField.FontColor = [0 1 0];
            app.EditField.BackgroundColor = [0.8 0.8 0.8];
            app.EditField.Position = [47 624 125 40];
            app.EditField.Value = '0:00';

            % Create RightPanel
            app.RightPanel = uipanel(app.GridLayout);
            app.RightPanel.BackgroundColor = [0 0 0];
            app.RightPanel.Layout.Row = 1;
            app.RightPanel.Layout.Column = 2;

            % Create FLabel
            app.FLabel = uilabel(app.RightPanel);
            app.FLabel.HorizontalAlignment = 'center';
            app.FLabel.FontSize = 15;
            app.FLabel.FontWeight = 'bold';
            app.FLabel.Position = [122 574 25 22];
            app.FLabel.Text = 'F';

            % Create TabGroup
            app.TabGroup = uitabgroup(app.RightPanel);
            app.TabGroup.Position = [6 6 730 730];

            % Create PowertrainTab
            app.PowertrainTab = uitab(app.TabGroup);
            app.PowertrainTab.Title = 'Powertrain';
            app.PowertrainTab.BackgroundColor = [0 0 0];

            % Create UIAxes2
            app.UIAxes2 = uiaxes(app.PowertrainTab);
            title(app.UIAxes2, 'Title')
            xlabel(app.UIAxes2, 'Time')
            ylabel(app.UIAxes2, 'Coolant Temp')
            zlabel(app.UIAxes2, 'Z')
            app.UIAxes2.XColor = [1 1 1];
            app.UIAxes2.YColor = [1 1 1];
            app.UIAxes2.Color = [0 0 0];
            app.UIAxes2.GridColor = [0 1 0];
            app.UIAxes2.MinorGridColor = [0 1 0];
            app.UIAxes2.Position = [23 469 323 219];

            % Create UIAxes2_2
            app.UIAxes2_2 = uiaxes(app.PowertrainTab);
            title(app.UIAxes2_2, 'Title')
            xlabel(app.UIAxes2_2, 'Time')
            ylabel(app.UIAxes2_2, 'Fuel Tank Temp')
            zlabel(app.UIAxes2_2, 'Z')
            app.UIAxes2_2.XColor = [1 1 1];
            app.UIAxes2_2.YColor = [1 1 1];
            app.UIAxes2_2.Color = [0 0 0];
            app.UIAxes2_2.GridColor = [0.15 0.15 0.15];
            app.UIAxes2_2.MinorGridColor = [0 1 0];
            app.UIAxes2_2.Position = [376 469 323 219];

            % Create UIAxes2_3
            app.UIAxes2_3 = uiaxes(app.PowertrainTab);
            title(app.UIAxes2_3, 'Title')
            xlabel(app.UIAxes2_3, 'Time')
            ylabel(app.UIAxes2_3, 'Radiator Temp')
            zlabel(app.UIAxes2_3, 'Z')
            app.UIAxes2_3.XColor = [1 1 1];
            app.UIAxes2_3.YColor = [1 1 1];
            app.UIAxes2_3.Color = [0 0 0];
            app.UIAxes2_3.GridColor = [0.15 0.15 0.15];
            app.UIAxes2_3.MinorGridColor = [0 1 0];
            app.UIAxes2_3.Position = [24 243 323 219];

            % Create UIAxes2_4
            app.UIAxes2_4 = uiaxes(app.PowertrainTab);
            title(app.UIAxes2_4, 'Title')
            xlabel(app.UIAxes2_4, 'Time')
            ylabel(app.UIAxes2_4, ' Seat Temp')
            zlabel(app.UIAxes2_4, 'Z')
            app.UIAxes2_4.XColor = [1 1 1];
            app.UIAxes2_4.YColor = [1 1 1];
            app.UIAxes2_4.Color = [0 0 0];
            app.UIAxes2_4.GridColor = [0.15 0.15 0.15];
            app.UIAxes2_4.MinorGridColor = [0 1 0];
            app.UIAxes2_4.Position = [377 244 323 219];

            % Create UIAxes2_5
            app.UIAxes2_5 = uiaxes(app.PowertrainTab);
            title(app.UIAxes2_5, 'Title')
            xlabel(app.UIAxes2_5, 'Time')
            ylabel(app.UIAxes2_5, 'Throttle Position')
            zlabel(app.UIAxes2_5, 'Z')
            app.UIAxes2_5.XColor = [1 1 1];
            app.UIAxes2_5.YColor = [1 1 1];
            app.UIAxes2_5.Color = [0 0 0];
            app.UIAxes2_5.GridColor = [0.15 0.15 0.15];
            app.UIAxes2_5.MinorGridColor = [0 1 0];
            app.UIAxes2_5.Position = [25 18 323 219];

            % Create UIAxes2_6
            app.UIAxes2_6 = uiaxes(app.PowertrainTab);
            title(app.UIAxes2_6, 'Title')
            xlabel(app.UIAxes2_6, 'Time')
            ylabel(app.UIAxes2_6, 'Brake Position')
            zlabel(app.UIAxes2_6, 'Z')
            app.UIAxes2_6.XColor = [1 1 1];
            app.UIAxes2_6.YColor = [1 1 1];
            app.UIAxes2_6.Color = [0 0 0];
            app.UIAxes2_6.GridColor = [0.15 0.15 0.15];
            app.UIAxes2_6.MinorGridColor = [0 1 0];
            app.UIAxes2_6.Position = [378 18 323 219];

            % Create ChassisTab
            app.ChassisTab = uitab(app.TabGroup);
            app.ChassisTab.Title = 'Chassis';

            % Create AeroTab
            app.AeroTab = uitab(app.TabGroup);
            app.AeroTab.Title = 'Aero';

            % Show the figure after all components are created
            app.UIFigure.Visible = 'on';
        end
    end

    % App creation and deletion
    methods (Access = public)

        % Construct app
        function app = BarrettCode

            % Create UIFigure and components
            createComponents(app)

            % Register the app with App Designer
            registerApp(app, app.UIFigure)

            if nargout == 0
                clear app
            end
        end

        % Code that executes before app deletion
        function delete(app)

            % Delete UIFigure when app is deleted
            delete(app.UIFigure)
        end
    end
end