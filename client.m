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
    'hardware', 'Timeout', 20);
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
        case 'a'
            %% CASE A: Read ADC (counts)
            n = fscanf(mySerial, '%d');             % get the ADC counts
            fprintf('ADC reading: %d counts\n', n); % print it to the screen
        case 'b'
            %% CASE B: Read ADC (milliamps)
            n = fscanf(mySerial, '%d');         % get the ADC counts
            fprintf('ADC reading: %d mA\n', n); % print it to the screen
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
        case 'f'
            %% CASE F: Set PWM between -100 and 100
            n = input('Enter Duty Cycle between -100 and 100: ');
            fprintf(mySerial, '%d\n', n);
            fprintf('\nDuty Cycle set to %d\n', n); 
        case 'g'
            %% CASE G: Set current gains
            n = input('Enter Current Gains [Kp, Ki]: ');
            fprintf(mySerial, '%f %f\n', [n(1), n(2)]);
            fprintf('Current gains set\n');
        case 'h'
            %% CASE H: Get current gains
            n = fscanf(mySerial, '%f %f');
            fprintf('Current Gains:\n\tKp: %f Ki: %f\n', [n(1), n(2)]);
        case 'i'
            %% CASE I: Set position gains
            n = input('Enter Position Gains [Kp, Ki, Kd]: ');
            fprintf(mySerial, '%f %f %f\n', [n(1), n(2), n(3)]);
            fprintf('Position gains set\n');
        case 'j'
            %% CASE J: Get position gains
            n = fscanf(mySerial, '%f %f %f');
            fprintf('Position Gains:\n\tKp: %f\n\tKi: %f\n\tKd: %f\n', [n(1), n(2), n(3)]);
        case 'k'
            %% CASE K: Test Current Gains
            nsamples = fscanf(mySerial, '%d');              % get the number of samples
            data = zeros(nsamples, 2);                      % two values per sample: ref and actual
            for i = 1:nsamples
                data(i, :) = fscanf(mySerial, '%d %d');     % read in data from PIC32, assume ints in mA
                times(i) = (i - 1) * 0.2;                   % 0.2 ms between samples
            end
            if nsamples > 1
                figure;
                stairs(times, data(:, 1:2));                % plot the reference and actual
            else
                fprintf('Only 1 sample received\n');
                disp(data);
            end
            % compute the average error
            score = mean(abs(data(:, 1) - data(:, 2)));
            fprintf('\nAverage error: %5.1f mA\n', score);
            title(sprintf('Average error: %5.1f mA\n', score));
            ylabel('Current (mA)');
            xlabel('Time (ms)');
        case 'l' 
            %% CASE L: Go to angle (degrees)
            n = input('Enter Desired Angle (degrees): ');
            fprintf(mySerial, '%f\n', n);
            fprintf('Angle Set\n');
        case 'm'
            %% CASE M: Load step trajectory
            n = input('Enter Desired Trajectory\n[t1,p1;...;tn,pn]: ');
            ref = genRef(n, 'step');
            ref_size = size(ref, 2);
            fprintf(mySerial, '%d\n', ref_size);
            fprintf('Array size: %d\n', ref_size);
            n = fscanf(mySerial, '%d');
            if n
                fprintf('Sending Array...\n');
                for i = 1:ref_size
                   fprintf(mySerial, '%f\n', ref(i)); 
                end
                done = fscanf(mySerial, '%d');
                if done
                    fprintf('Finished sending trajectory\n');
                end
            else
                fprintf('ARRAY TOO BIG');
            end
        case 'n'
            %% CASE N: Load cubic trajectory
            n = input('Enter Desired Trajectory\n[t1,p1;...;tn,pn]: ');
            ref = genRef(n, 'cubic');
            ref_size = size(ref, 2);
            fprintf(mySerial, '%d\n', ref_size);
            fprintf('Array size: %d\n', ref_size);
            n = fscanf(mySerial, '%d');
            if n
                fprintf('Sending Array...\n');
                for i = 1:ref_size
                   fprintf(mySerial, '%f\n', ref(i)); 
                end
                done = fscanf(mySerial, '%d');
                if done
                    fprintf('Finished sending trajectory\n');
                end
            else
                fprintf('ARRAY TOO BIG');
            end
        case 'o'
            %% CASE O: Execute trajectory
            fprintf('Executing trajectory...\n');
            nsamples = fscanf(mySerial, '%d');              % get the number of samples
            data = zeros(nsamples, 2);                      % two values per sample: ref and actual
            for i = 1:nsamples
                data(i, :) = fscanf(mySerial, '%f %f');     % read in data from PIC32, assume floats in degrees
                times(i) = (i - 1) * 0.005;                 % 0.005 s between samples
            end
            if nsamples > 1
                figure;
                stairs(times, data(:, 1:2));                % plot the reference and actual
            else
                fprintf('Only 1 sample received\n');
                disp(data);
            end
            ylabel('Angle (degrees)');
            xlabel('Time (s)');
        case 'p'
            %% CASE P: Unpower the motor
            fprintf('Motor powered down\n');
        case 'q'
            %% CASE Q: Quit
            has_quit = true;    % exit client
        case 'r'
            %% CASE R: Get mode
            n = fscanf(mySerial, '%d');         % get the current mode
            fprintf('Current mode: %d\n', n);   % print it to the screen
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
end

end