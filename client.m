function client(port)
%   provides a menu for accessing PIC32 motor control functions
%
%   client(port)
%
%   Input Arguments: 
%       port - the name of the COM port. This should be the same as what
%       you use in screen or putty in quotes ' '
%
%   Example:
%       client('/dev/ttyUSB0') (Linux/Mac)
%       client('COM3') (PC)
%
%   For convenience, you may want to change this so that the port is
%   hardcoded.

%% Opening COM Connection
if ~isempty(instrfind)
    fclose(instrfind);
    delete(instrfind);
end

fprintf('Opening port %s....\n', port);

% Serial Port Settings
mySerial = serial(port, 'BaudRate', 230400, 'FlowControl',...
    'hardware', 'Timeout', 120);
% Opens serial connection
fopen(mySerial);    
% closes serial port when function exits
clean = onCleanup(@()fclose(mySerial));

has_quit = false;
%% BEGIN MENU LOOP
while ~has_quit
    fprintf('\n\nPIC32 MOTOR DRIVER INTERFACE\n\n');
    % display the menu options; this list will grow
    fprintf('\ta: Read current sensor (ADC Counts)    b: Read current sensor (mA)\n');
    fprintf('\tc: Read encoder (counts)               d: Read encoder (deg)\n');
    fprintf('\te: Reset encoder                       f: Set PWM (-100 to 100)\n');
    fprintf('\tg: Set current gains                   h: Get current gains\n');
    fprintf('\ti: Set position gains                  j: Get position gains\n');
    fprintf('\tk: Test current control                l: Go to angle (deg)\n');
    fprintf('\tm: Load step trajectory                n: Load cubic trajectory\n');
    fprintf('\to: Execute trajectory                  p: Unpower the motor\n');
    fprintf('\tq: Quit client                         r: Get mode\n');
    % read the user's choice
    selection = input('\nENTER COMMAND: ', 's');
    
    % send the command to the PIC32
    fprintf(mySerial, '%c\n', selection);
    
    %% SWITCH MENU
    switch selection
        case 'c'
            %% CASE C: Read encoder (counts)
            n = fscanf(mySerial, '%d');                     % get the encoder counts
            fprintf('Encoder Position: %d counts\n', n);  % print it to the screen
        case 'd'  
            %% CASE D: Read encoder (degrees)
            n = fscanf(mySerial, '%d');                             % get the angle in 1/10th degrees
            fprintf('Encoder Position: %4.1f degrees\n', (n / 10));    % print it to the screen
        case 'e'
            %% CASE E: Reset encoder position
            fprintf('Encoder Position Reset to 0\n');   % print message to screen
        case 'q'
            %% CASE Q: Quit
            has_quit = true;    % exit client
        case 'x'
            %% CASE X: Add Two Integers
            x = input('Enter two numbers [x y]: ');    % get numbers to send
            fprintf(mySerial, '%d %d\n', [x(1), x(2)]);  % send numbers
            n = fscanf(mySerial, '%d');                % get the added numbers back
            fprintf('x + y = %d\n', n);                  % print it to the screen
        otherwise
            %% DEFAULT CASE
            fprintf('Invalid Selection %c\n', selection);
    end
    if ~has_quit
        fprintf('Press any key to continue...');
        pause;
    end
end

end