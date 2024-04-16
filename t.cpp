//for running the game use the following cmd in linux after installing sfml in linux using command -> sudo apt-get install libsfml-dev
//for compiling the program use command ->  g++ t.cpp -lsfml-graphics -lsfml-window -lsfml-system -o t  
//to run the executible use command -> ./t 

#include <iostream>  //l226726   Maaz Khan
#include <unistd.h>
#include <pthread.h>
#include <string>
#include <SFML/Graphics.hpp>

using namespace std;

// GLOBAL VARIABLES
int paddle1_movement = 0;
int paddle2_movement = 0;
int playerOption = 0;
bool ballWaiting = false;
bool pointScored = false;
bool gameWon = false;
int serviceTurn = 1;
int paddle1ScoreCount=0;
int paddle2ScoreCount=0;
int ballSpeed=0;

// Define structs to hold arguments for the threads functions
struct Thread1Args   //for ball
{
    sf::Vector2f* ballPosition;
    sf::Vector2f* ballVelocity;
    sf::RenderWindow* window;
    sf::Vector2f* paddle1Position;
    sf::Vector2f* paddle2Position;
};

struct Thread2Args   //for paddle1
{
    sf::Vector2f* paddle1Position;
    sf::RenderWindow* window;
};

struct Thread3Args   //for paddle2
{
    sf::Vector2f* paddle2Position;
    sf::RenderWindow* window;
};


void* moveBall(void* arg)
{
    Thread1Args* args = static_cast<Thread1Args*>(arg);

    while (args->window->isOpen())
    {
     
        // Update ball position
        if(!ballWaiting)
        {
        *(args->ballPosition) += *(args->ballVelocity);
        }
        

        if (args->ballPosition->y <= 0 || args->ballPosition->y >= args->window->getSize().y)
        {
            args->ballVelocity->y = -args->ballVelocity->y; // Reverse vertical velocity on wall collision
        }

        // Check for collisions with paddles
        sf::Vector2f ballPos = *(args->ballPosition);
        sf::Vector2f paddle1Pos = *(args->paddle1Position);
        sf::Vector2f paddle2Pos = *(args->paddle2Position);

        // Collision with paddle1  //30 is the width of the paddle so we are checking touchness of the ball
        if (ballPos.x >= paddle1Pos.x - 10 && ballPos.x <= paddle1Pos.x + 30 && ballPos.y >= paddle1Pos.y && ballPos.y <= paddle1Pos.y + 200)
        {
            // Calculate the reflection angle and adjust ball velocity
            if(ballPos.y >= paddle1Pos.y && ballPos.y <= paddle1Pos.y + 60)
            {
                args->ballVelocity->x = -1;
                args->ballVelocity->y = -1;
            }
            else if(ballPos.y > paddle1Pos.y + 60 && ballPos.y <= paddle1Pos.y + 130){
            args->ballVelocity->x = -1;
            args->ballVelocity->y = 0;
            }
            else if(ballPos.y > paddle1Pos.y + 130 && ballPos.y <= paddle1Pos.y + 200){
            args->ballVelocity->x = -1;
            args->ballVelocity->y = 1; // Adjust ball speed as needed
            }
        }

        // Collision with paddle2
        if (ballPos.x <= paddle2Pos.x + 30 && ballPos.x >= paddle2Pos.x && ballPos.y >= paddle2Pos.y && ballPos.y <= paddle2Pos.y + 200)
        {
            // Calculate the reflection angle and adjust ball velocity
            if(ballPos.y >= paddle2Pos.y && ballPos.y <= paddle2Pos.y + 60)
            {
                args->ballVelocity->x = 1;
                args->ballVelocity->y = -1;
            }
            else if(ballPos.y > paddle2Pos.y + 60 && ballPos.y <= paddle2Pos.y + 130){
            args->ballVelocity->x = 1;
            args->ballVelocity->y = 0;
            }
            else if(ballPos.y > paddle2Pos.y + 130 && ballPos.y <= paddle2Pos.y + 200){
            args->ballVelocity->x = 1;
            args->ballVelocity->y = 1; // Adjust ball speed as needed
            }
        }

        // Check if the ball is missed by the paddles
        if(args->ballPosition->x <= 0 || args->ballPosition->x >= args->window->getSize().x)
        {
             // Set pointScored to true to indicate that a point has been scored
            pointScored = true;
            // Set ballWaiting to true to stop the ball movement
            ballWaiting = true;

            if(ballPos.x<=0)
            {
                serviceTurn=1;
                paddle1ScoreCount++;
            }
            else
            {
                serviceTurn=2;
                paddle2ScoreCount++;
            }


            // Reset the ball position to the center
            args->ballPosition->x = args->window->getSize().x / 2;
            args->ballPosition->y = args->window->getSize().y / 2;

            //whoever makes point the next service will be from him...
            if(serviceTurn==1)
            {
                args->ballVelocity->x = 1; // Reset ball velocity
                args->ballVelocity->y = 0; // Reset ball velocity
            }
            else{
                args->ballVelocity->x = -1; // Reset ball velocity
                args->ballVelocity->y = 0; // Reset ball velocity
            }

            //or we can use this syntax for doing the same thing i.e setting ball velocity for service appropriately..
            //args->ballVelocity->x=serviceTurn==1 ? 1:-1;   //it will return these options | first option is for true case and second option is for false case..
            //args->ballvelocity->y=0;

            

            // Delay for displaying point label
            usleep(1000000); //1 second

            if(paddle1ScoreCount==5 || paddle2ScoreCount==5)
            {
                gameWon=true;
                usleep(3000000);
                // Reset the scores
                paddle1ScoreCount = 0;
                paddle2ScoreCount = 0;
                gameWon=false;
            }

            // Reset pointScored and ballWaiting after delay
            pointScored = false;
            ballWaiting = false;

        }
        
        if(ballSpeed==1)
        // Sleep for a short duration to control ball speed
        usleep(900); // Adjust as needed for controlling the motion or speed of the ball..
        else
            usleep(500);

    }

    return nullptr;
}

void* movePaddle1(void* arg)
{
    Thread2Args* args = static_cast<Thread2Args*>(arg);

    while (args->window->isOpen())
    {
        //moving paddle
        if (paddle1_movement==-1) 
        {
            args->paddle1Position->y -= 1;
        }
        if (paddle1_movement==1) 
        {
            args->paddle1Position->y += 1;
        }

        // Sleep to avoid consuming excessive CPU resources
        usleep(500); // Adjust as needed
    }

    return nullptr;
}

void* movePaddle2(void* arg)
{
    Thread3Args* args = static_cast<Thread3Args*>(arg);

    
     

    while (args->window->isOpen())
    {
        if(playerOption==2)
        {
        //moving paddle
        if (paddle2_movement==-1) 
        {
            args->paddle2Position->y -= 1;
        }
        if (paddle2_movement==1) 
        {
            args->paddle2Position->y += 1;
        }

        // Sleep to avoid consuming excessive CPU resources
        usleep(500); // Adjust as needed
        }

        else if(playerOption == 1)    //automatically move paddle up and down randomly....
        {
           bool movingUp=true;
           bool movingDown=false;

           while(args->window->isOpen())  //we havenot done while(1) because it does not terminate and terminal have to be close and open again to be for use...
           {
            if(movingUp)
            {
                args->paddle2Position->y -= 1;
                if(args->paddle2Position->y < 0)
                {
                    movingDown=true;
                    movingUp=false;
                }
            usleep(200);
            }
            if(movingDown)
            {
                args->paddle2Position->y += 1;
                if(args->paddle2Position->y > 815)
                {
                    movingUp=true;
                    movingDown=false;
                }
            usleep(200);
            }
            
            }

        }

        
    }

    return nullptr;
}

int main()
{
    cout << "\n\n_________WELCOME TO PONG GAME!_________\n\nINPUTS ( 1 -> single player | 2 -> two player )\nChoice: ";
    while (cin >> playerOption)
    {
        if (playerOption == 1 || playerOption == 2)
            break;
        else
            cout << "\n\nError: invalid input made\n input again : ";
    }
    cout<<"Alright! Now select ball motion speed :\n( 1 -> slow  |  2 -> fast )\nChoice: ";
     while (cin >> ballSpeed)
    {
        if (ballSpeed == 1 || ballSpeed == 2)
            break;
        else
            cout << "\n\nError: invalid input made\n input again : ";
    }

    // Create the game window
    sf::RenderWindow window(sf::VideoMode(1900, 1000), "- P O N G -");

    // Initialize ball position and velocity
    sf::Vector2f ballPosition(window.getSize().x / 2, window.getSize().y / 2);
    sf::Vector2f ballVelocity(1, 0); // Adjust velocity as needed  //initial direction of the ball..

    sf::Vector2f paddle1Position(1890, ((window.getSize().y / 2) - 100));   //-100 for completely in middle vertically..
    sf::Vector2f paddle2Position(0, ((window.getSize().y / 2) - 100)); //-100 for completely in middle vertically..

    // Draw game objects
    sf::CircleShape ball(10);
    sf::RectangleShape paddle1(sf::Vector2f(30, 200));
    sf::RectangleShape paddle2(sf::Vector2f(30, 200));

    sf::Font font;
    if(!font.loadFromFile("Bonza.otf")) //load font from file
    {
        return EXIT_FAILURE;
    }

    sf::Text scoreLabel;
    scoreLabel.setFont(font);
    scoreLabel.setString("( SCORE )");
    scoreLabel.setCharacterSize(30);
    scoreLabel.setFillColor(sf::Color::White);
    scoreLabel.setPosition(910,10);

    

    // Create a thread1 argument struct
    Thread1Args args1;
    args1.ballPosition = &ballPosition;
    args1.ballVelocity = &ballVelocity;
    args1.paddle1Position = &paddle1Position;
    args1.paddle2Position = &paddle2Position;
    args1.window = &window;

    pthread_t thread1;
    pthread_create(&thread1, nullptr, moveBall, &args1);

    // Create a thread2 argument struct
    Thread2Args args2;
    args2.paddle1Position = &paddle1Position;
    args2.window = &window;

    pthread_t thread2;
    pthread_create(&thread2, nullptr, movePaddle1, &args2);

    // Create a thread3 argument struct
    Thread3Args args3;
    args3.paddle2Position = &paddle2Position;
    args3.window = &window;

    pthread_t thread3;
    pthread_create(&thread3, nullptr, movePaddle2, &args3);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
        }


        ball.setFillColor(sf::Color::Red);
        ball.setPosition(ballPosition);

        paddle1.setFillColor(sf::Color::Green);
        paddle1.setPosition(paddle1Position);

        paddle2.setFillColor(sf::Color::Blue);
        paddle2.setPosition(paddle2Position);

        // Handle paddle1 movement
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) && paddle1Position.y > 0) 
        {
            paddle1_movement=-1;
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) && paddle1Position.y < 815) 
        {
            paddle1_movement=1;
        }
        else{
            paddle1_movement=0;
        }

        if(playerOption==2)
        {
        // Handle paddle2 movement
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) && paddle2Position.y > 0) 
        {
            paddle2_movement=-1;
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) && paddle2Position.y < 815) 
        {
            paddle2_movement=1;
        }
        else{
            paddle2_movement=0;
        }

        }


        // Clear the window
        window.clear(sf::Color::Black);

        string paddle1ScoreStr = std::to_string(paddle1ScoreCount);
        sf::Text paddle1ScoreInteger;
        paddle1ScoreInteger.setFont(font);
        paddle1ScoreInteger.setString(paddle1ScoreStr);
        paddle1ScoreInteger.setCharacterSize(25);
        paddle1ScoreInteger.setFillColor(sf::Color::Green);
        paddle1ScoreInteger.setPosition(1000,60);
    
        string paddle2ScoreStr = std::to_string(paddle2ScoreCount);
        sf::Text paddle2ScoreInteger;
        paddle2ScoreInteger.setFont(font);
        paddle2ScoreInteger.setString(paddle2ScoreStr);
        paddle2ScoreInteger.setCharacterSize(25);
        paddle2ScoreInteger.setFillColor(sf::Color::Blue);
        paddle2ScoreInteger.setPosition(907,60);
        
        if(!ballWaiting)
        {
        window.draw(ball);    
        }
        else
        {

        if (gameWon)
        {
        // Display the victory message
        sf::Text victoryMessage;
        victoryMessage.setFont(font);
        victoryMessage.setCharacterSize(100);
        victoryMessage.setFillColor(sf::Color::Yellow);
        victoryMessage.setPosition(750, 400);

        if (paddle1ScoreCount == 5)
        {
            victoryMessage.setString("Paddle 1 WON!");
        }
        else
        {
            victoryMessage.setString("Paddle 2 WON!");
        }

        window.draw(victoryMessage);
        }
        else
        {
            // Display the point message
            sf::Text pointLabel;
            pointLabel.setFont(font);
            pointLabel.setString("POINT!");
            pointLabel.setCharacterSize(100);
            pointLabel.setFillColor(sf::Color::Red);
            pointLabel.setPosition(855, 400);
            window.draw(pointLabel);
        }

        }

        window.draw(paddle1);
        window.draw(paddle2);
        window.draw(scoreLabel);
        window.draw(paddle1ScoreInteger);
        window.draw(paddle2ScoreInteger);

        window.display();
    }

    // Wait for the threads to finish
    pthread_join(thread1, nullptr);
    pthread_join(thread2, nullptr);
    pthread_join(thread3, nullptr);

    return 0;
}
