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
    fprintf('PIC32 MOTOR DRIVER INTERFACE\n\n');
    % display the menu options; this list will grow
    fprintf('\td: Dummy Command\tq: Quit\n');
    % read the user's choice
    selection = input('\nENTER COMMAND: ', 's');
    
    % send the command to the PIC32
    fprintf(mySerial, '%c\n', selection);
    
    %% SWITCH MENU
    switch selection
        case 'd'  
            %% CASE D: Dummy Operation
            n = input('Enter number: ');    % get the number to send
            fprintf(mySerial, '%d\n', n);   % send the number
            n = fscanf(mySerial, '%d');     % get the incremented number back
            fprintf('Read: %d\n', n);       % print it to the screen
        case 'q'
            %% CASE Q: Quit
            has_quit = true;    % exit client
        otherwise
            %% DEFAULT CASE
            fprintf('Invalid Selection %c\n', selection);
    end
end

end