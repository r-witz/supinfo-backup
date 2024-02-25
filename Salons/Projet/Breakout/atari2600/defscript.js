$(document).ready(function () {

    //Game Vars
    bricksList = [];
    ballsList = [];
    bonusBallsHistory = 0; //Compte le nombre de fois où une balle bonus a été générée
    gameplaying = false;
    FPSinMS = 8;
    verticalCounter = 0; //Compte le nombre de rebonds successifs de la balle sur la barre avec un angle de 90°
    isBonusFalling = false;
    points = 0;
    lives = 3;

    //Size Vars
    nbrRow = 11;
    nbrColumns = 8;

    //Selectors Vars
    $gameInfo = $("#gameInfo");
    $gameArea = $("#gameArea");
    $playingBar = $("#playingBar"); //$playingBarPosition defined in function placePlayingBar

    //Position Vars
    $gameAreaPosition = {
        "Left": $gameArea.position().left,
        "Top": $gameArea.position().top,
        "Right": $gameArea.position().left + $gameArea.width(),
        "Bottom": $gameArea.position().top + $gameArea.height()
    };


    /** Génère le tableau (avec les ID de chaque brique etc)
     * -----------
     * APPELLE buildBreakableList()
     * APPELLE placePlayingBar()
     */
    function generateBoard() {
        brickID = 0;
        ballsList.push({
            "Selector": $("#ball"),
            "Top": 0,
            "Left": 0,
            "Angle": undefined
        });

        for (var currentRow = 0; currentRow < nbrRow; currentRow++) {
            $("#plateau").append("<tr id='row" + currentRow + "'></tr>");
            rndColor = "rgb(" + randomColor()[0] + ", " + randomColor()[1] + ", " + randomColor()[2] + ")";
            for (var currentColumn = 0; currentColumn < nbrColumns; currentColumn++) {
                brickID++;
                generateCell = Math.floor(Math.random() * 100);
                if (generateCell <= 20) {
                    $("#row" + currentRow).append("<td id='black" + brickID + "'></td>");
                    $("#black" + brickID).css("background-color", "rgba(0, 0, 0, 0)");
                } else {
                    $("#row" + currentRow).append("<td id='" + brickID + "'></td>");
                    $newBrick = $("#" + brickID);
                    $newBrick.css("background-color", rndColor);
                }
            }
        }
        buildBreakableList();
        placePlayingBar();
    }

    // Ajoute dans une liste les informations de chaque brique pouvant être cassées
        function buildBreakableList(){
            brickID = 0;
            $("td").each(function () {
                brickID++;
                if ($(this).css("backgroundColor") !== "rgba(0, 0, 0, 0)") {
                    brickInfo = {
                        "Top": $(this).offset().top,
                        "Left": $(this).offset().left,
                        "Right": $(this).offset().left + $(this).width(),
                        "Bottom": $(this).offset().top + $(this).height(),
                        "ID": brickID
                    };
                    bricksList.push(brickInfo);
                }
            });
        }

    // Place la barre et la balle au centre de la page
    function placePlayingBar() {
        $playingBar.css("top", ($gameAreaPosition.Bottom - $gameAreaPosition.Top - $playingBar.height()) - 20);    //Remonte la barre à un niveau de jeu dans le plateau acceptable
        $playingBar.css("left", ($gameArea.width()/2 - $playingBar.width()/2));     //Centre la barre
        $playingBarPosition = {
            "Left": $playingBar.offset().left,
            "Top": $playingBar.offset().top,
            "Right": $playingBar.offset().left + $playingBar.width(),
            "Bottom": $playingBar.offset().top + $playingBar.height(),
            "Width": $playingBar.width(),
            "Height": $playingBar.height()
        };
        ballsList[0].Selector.css({
            "top": $playingBarPosition.Top - ballsList[0].Selector.height(),
            "left": $playingBarPosition.Left + (($playingBar.width()/2 - ballsList[0].Selector.width()/2))
        }); //Centre la balle sur la barre

    }

    //Surveille les mouvements de la souris
    function monitorMouseMove(){
        $("body").mousemove(function (e) {
            calculatedLeft = e.pageX - $gameAreaPosition.Left +70 - ($playingBarPosition.Width*2);
            calculatedRight = calculatedLeft + $playingBarPosition.Width;
            if (0 < calculatedLeft && calculatedRight < $gameArea.width()) {
                console.log("entered");
                $playingBar.css("left", calculatedLeft);
                $playingBarPosition = {
                    "Left": $playingBar.offset().left,
                    "Top": $playingBar.offset().top,
                    "Right": $playingBar.offset().left + $playingBar.width(),
                    "Bottom": $playingBar.offset().top + $playingBar.height(),
                    "Width": $playingBar.width(),
                    "Height": $playingBar.height()
                };
            }
        });
    }

    // Génère une couleur aléatoire pour chaque Brique
    function randomColor() {
        red = Math.floor((Math.random() * 256));
        green = Math.floor((Math.random() * 256));
        blue = Math.floor((Math.random() * 256));
        return [red, green, blue];
    }

    // Récupère les nouvelles coordonnées de la balle et l'indique au CSS
    function movingball(launching, ballNumber) {
        x = ballsList[ballNumber].Left;
        y = ballsList[ballNumber].Top;
        angle = ballsList[ballNumber].Angle;

        if (launching === true) {
            //    Lancer à 45° à gauche ou à droite (1 chance sur 2)
            direction = Math.floor(Math.random()*2)+1;
            if (direction === 1){
                ballsList[ballNumber].Angle = 45;
            }else{
                ballsList[ballNumber].Angle = 135;
            }
            launching = false;
        }

        //    Algorithme mouvance de la balle
        angle = checkCollision(ballsList[ballNumber].Selector, ballsList[ballNumber].Angle, ballNumber);

        //Si l'angle est égal à -1 c'est que la balle a disparue. On annule son déplacement
        if (angle !== -1){
            // La condition ci-dessous change la valeur FPSinMS pour réguler la vitesse de la balle
            if ((angle > 0 && angle <= 20)
                || (angle >= 70 && angle <= 110)
                || (angle >= 160 && angle <= 200)
                || (angle >= 250 && angle <= 290)
                || (angle >= 340 && angle < 360)) {
                FPSinMS = 4;
            } else {
                FPSinMS = 8;
            }

            //Calcule la nouvelle position de la balle ET la replace de force si elle a réussi à passer un mur malgré les limitations
            try {
                calculatedBallTop = ballsList[ballNumber].Selector.position().top + calcTraj(recoverBallPosition(x, y, angle, ballsList[ballNumber].Selector)[0], recoverBallPosition(x, y, angle, ballsList[ballNumber].Selector)[1], recoverBallPosition(x, y, angle, ballsList[ballNumber].Selector)[2]).decalY;
                calculatedBallLeft = ballsList[ballNumber].Selector.position().left + calcTraj(recoverBallPosition(x, y, angle, ballsList[ballNumber].Selector)[0], recoverBallPosition(x, y, angle, ballsList[ballNumber].Selector)[1], recoverBallPosition(x, y, angle, ballsList[ballNumber].Selector)[2]).decalX;
            } catch(e){}
            //Déplace la Balle sur le plateau
            ballsList[ballNumber].Selector.css({
                "top": calculatedBallTop,
                "left": calculatedBallLeft
            });

            //Actualise les informations de la balle "BallNumber"
            ballsList[ballNumber] = {
                "Selector": ballsList[ballNumber].Selector,
                "Top": calculatedBallTop,
                "Left": calculatedBallLeft,
                "Angle": angle
            };
        }

    }

    // Remet la balle dans le tableau si elle a dépassé la bordure
    function recoverBallPosition(x, y, angle, ball) {
        if (x < $gameAreaPosition.Left + 72) { // remet la balle dans le tableau si elle a dépassé la bordure
            x = $gameAreaPosition.Left + 70;
            if (angle > 0 && angle < 90) {
                angle = 180 - (90 - angle);
            } else if (angle > 270 && angle < 360) {
                angle = 180 + (360 - angle);
            }
        } else if (x + ball.width() >= $gameAreaPosition.Right + 70) {   // remet la balle dans le tableau si elle a dépassé la bordure
            x = $gameAreaPosition.Right + 68;
            if (angle > 90 && angle < 180) {
                angle = 90 - (angle - 90);
            } else if (angle > 180 && angle < 270) {
                angle = 360 - (angle - 180);
            }
        }
        return [x, y, angle];
    }

    // Indique le nombre de pixel de décalage en fonction de l'angle de la trajectoire
    function calcTraj(x, y, angle) {
        if (angle >= 360) {
            angle = angle - 360;
        }
        if (angle === 0 || angle === 180) {
            //ERROR
        } else if (angle === 90) {
            return {
                "decalX": 0,
                "decalY": -3
            }
        } else if (angle === 270) {
            return {
                "decalX": 0,
                "decalY": 3
            }
        } else if (angle <= 45) {
            return {
                "decalX": -3,
                "decalY": (angle / 45) * (-3)
            }
        } else if (angle > 45 && angle < 90) {
            return {
                "decalX": ((90 - angle) / (-45)) * 3,
                "decalY": -3
            }
        } else if (angle > 90 && angle <= 135) {
            return {
                "decalX": (angle / 135) * 3,
                "decalY": -3
            }
        } else if (angle > 135 && angle < 180) {
            return {
                "decalX": 3,
                "decalY": ((180 - angle) / 45) * (-3)
            }
        } else if (angle > 180 && angle <= 225) {
            return {
                "decalX": 3,
                "decalY": ((angle - 180) / 45) * 3
            }
        } else if (angle > 225 && angle < 270) {
            return {
                "decalX": ((90 - (angle - 180)) / 45) * 3,
                "decalY": 3
            }
        } else if (angle > 270 && angle <= 315) {
            return {
                "decalX": ((angle - 180) / 135) * (-3),
                "decalY": 3
            }
        } else if (angle > 315 && angle < 360) {
            return {
                "decalX": -3,
                "decalY": ((180 - (angle - 180)) / 45) * 3
            }
        }

    }

    // Permet de définir s'il y a eu collision, avec quoi et retourne le nouvel angle de la balle
    function checkCollision(ball, angle, ballNumber) {
        x = ballsList[ballNumber].Left;
        y = ballsList[ballNumber].Top;
        max = bricksList.length;

        //    check collision haut de la balle avec une brique
        for (var nbrBriques = 0; nbrBriques < max; nbrBriques++) {
            if (((y <= bricksList[nbrBriques].Bottom + 2 && y >= bricksList[nbrBriques].Bottom - 2) //check rebond brique côté bas
                    && (x + ballsList[ballNumber].Selector.width() >= bricksList[nbrBriques].Left - 2 && x < bricksList[nbrBriques].Right + 2))) {
                if (angle > 90 && angle < 180) {
                    angle = 180 + (180 - angle);
                } else if (angle > 0 && angle <= 90) {
                    angle = 360 - angle;
                }
                brickTouched(nbrBriques);
                return angle;
            } else if ((y + ballsList[ballNumber].Selector.height() <= bricksList[nbrBriques].Top + 2 && y + ballsList[ballNumber].Selector.height() >= bricksList[nbrBriques].Top - 2) // check rebond brique côté haut
                && (x + ballsList[ballNumber].Selector.width() >= bricksList[nbrBriques].Left + 2 && x <= bricksList[nbrBriques].Right - 2)) {
                if (angle > 180 && angle < 270) {
                    angle = 90 + (270 - angle);
                } else if (angle >= 270 && angle < 360) {
                    angle = 360 - angle;
                }
                brickTouched(nbrBriques);
                return angle;
            } else if ((y + ballsList[ballNumber].Selector.height() >= bricksList[nbrBriques].Top - 2 && y <= bricksList[nbrBriques].Bottom + 2) // check rebond brique côté gauche
                && (x + ballsList[ballNumber].Selector.width() >= bricksList[nbrBriques].Left - 2 && x + ballsList[ballNumber].Selector.width() <= bricksList[nbrBriques].Left + 2)) {
                if (angle > 90 && angle < 180) {
                    angle = 90 - (angle - 90);
                } else if (angle > 180 && angle < 270) {
                    angle = 360 - (angle - 180);
                }
                brickTouched(nbrBriques);
                return angle;
            } else if ((y + ballsList[ballNumber].Selector.height() >= bricksList[nbrBriques].Top - 2 && y <= bricksList[nbrBriques].Bottom + 2) // check rebond brique côté droit
                && (x <= bricksList[nbrBriques].Right + 2 && x >= bricksList[nbrBriques].Right - 2)) {
                if (angle > 0 && angle < 90) {
                    angle = 180 - angle;
                } else if (angle > 270 && angle < 360) {
                    angle = 180 + (360 - angle);
                }
                brickTouched(nbrBriques);
                return angle;
            }
        }

        if (x < $gameAreaPosition.Left + 72 && x > $gameAreaPosition.Left + 68) { // check les rebonds sur la paroi gauche
            if (angle > 0 && angle < 90) {
                angle = 180 - angle;
            } else if (angle > 270 && angle < 360) {
                angle = 180 + (360 - angle);
            }
            wallAudio = new Audio('wallcollision.mp3');
            wallAudio.play();
            return angle;

        } else if (x + ball.width() >= $gameAreaPosition.Right + 70 - 2 && x + ball.width() <= $gameAreaPosition.Right + 70 + 2) {   // check les rebonds sur la paroi droite
            if (angle > 90 && angle < 180) {
                angle = 90 - (angle - 90);
            } else if (angle > 180 && angle < 270) {
                angle = 360 - (angle - 180);
            }
            wallAudio = new Audio('wallcollision.mp3');
            wallAudio.play();
            return angle;

        } else if (($playingBarPosition.Top-2 <= y && y <= $playingBarPosition.Top+2)
            && ($playingBarPosition.Left - 3 <= x + ball.width() && x <= ($playingBarPosition.Right + 3))) {    // Check les collisions avec la barre
            if ((x + ball.width() < $playingBarPosition.Left + ($playingBarPosition.Width / 5) && (x + ball.width() > $playingBarPosition.Left - 3))) {
                angle = 20;
                barAudio = new Audio('barcollision.mp3');
                barAudio.play();
                return angle;
            } else if ((x > $playingBarPosition.Right - ($playingBar.width() / 5)) && (x < $playingBarPosition.Right)) {
                angle = 160;
                barAudio = new Audio('barcollision.mp3');
                barAudio.play();
                return angle;
            } else if ((x > $playingBarPosition.Left + ($playingBarPosition.Width / 2) - ($playingBar.width() / 10)) && (x < $playingBarPosition.Left + ($playingBarPosition.Width / 2) + ($playingBarPosition.Width / 10))) {
                verticalCounter++;
                if (verticalCounter > 3) {
                    angle = Math.floor(Math.random() * 170) + 10;
                    verticalCounter = 0;
                } else {
                    angle = 90;
                }
                barAudio = new Audio('barcollision.mp3');
                barAudio.play();
                return angle;
            } else if ((x > $playingBarPosition.Left + ($playingBarPosition.Width / 2) - ($playingBar.width() / 6)) && (x < $playingBarPosition.Left + ($playingBarPosition.Width / 2) + ($playingBarPosition.Width / 6))) {
                if (angle > 180 && angle <= 270) {
                    angle = 135;
                    barAudio = new Audio('barcollision.mp3');
                    barAudio.play();
                    return angle;
                } else if (angle > 270 && angle < 360) {
                    angle = 45;
                    barAudio = new Audio('barcollision.mp3');
                    barAudio.play();
                    return angle;
                }
            } else {
                if (angle > 180 && angle < 270) {
                    angle = 90 + (270 - angle);
                    barAudio = new Audio('barcollision.mp3');
                    barAudio.play();
                    return angle;
                } else if (angle > 270 && angle < 360) {
                    angle = 360 - angle;
                    barAudio = new Audio('barcollision.mp3');
                    barAudio.play();
                    return angle;
                } else if (angle === 270) {
                    angle = 45;
                    barAudio = new Audio('barcollision.mp3');
                    barAudio.play();
                    return angle;
                }
            }

        } else if (y <= bricksList[0].Top + 2 && y >= bricksList[0].Top - 2) { // Check les collisions avec le haut du plateau
            if (angle > 90 && angle < 180) {
                angle = 180 + (180 - angle);
            } else if (angle > 0 && angle <= 90) {
                angle = 360 - angle;
            }
            wallAudio = new Audio('wallcollision.mp3');
            wallAudio.play();
            return angle;
        }

        // checke la sortie de la balle
        if ($playingBarPosition.Bottom + $playingBarPosition.Height <= y - 2) {
            brickAudio = new Audio('brickcollision.mp3');
            brickAudio.play();
            if (ballsList.length > 1) {
                ballsList[ballNumber].Selector.remove();
                ballsList.splice(ballNumber, 1);
                return -1;
            } else {
                if (lives > 1) {
                    lives--;
                    lifeLost();
                } else {
                    lives--;
                    lifeLost();
                    ballsList[ballNumber].Selector.remove();
                    $("#gameEnd").css("display", "flex");
                }
            }
        }
        return angle;
    }

    function lifeLost(){
        gameplaying = false;
        refreshLiveCount();
        placePlayingBar();
        if (isBonusFalling === true) {
            clearInterval(fallbon);
            $bonusBrick.remove();
        }
        waitingForClick();
    }

    /** Rend la brique touchée transparente et l'enlève de la liste de briques à détruire
     * --------------
     * APPELLE refreshPointsCount();
     * APPELLE birthOfBonus(nbrBriques);
     */
    function brickTouched(nbrBriques){
        brickAudio = new Audio('brickcollision.mp3');
        brickAudio.play();
        $("#" + bricksList[nbrBriques].ID).css("backgroundColor", "rgba(0, 0, 0, 0)");
        refreshPointsCount();
        birthOfBonus(nbrBriques);
        bricksList.splice(nbrBriques, 1);
        if (bricksList.length === 0) {
            $("#gameEnd").css("display", "flex");
        }
    }

    // Affiche le nombre de vie(s) restantes
    function refreshLiveCount() {
        if (lives < 10) {
            $("#Lives").text("0" + lives);
        } else {
            $("#Lives").text(lives);
        }
    }

    //Ajoute et Affiche le nombre de points
    function refreshPointsCount() {
        points += 10;
        pointsLabel = Array(7 - points.toString().length).join("0") + points.toString();
        $("#Points").text(pointsLabel);
    }

    // Crée ou non un Bonus aléatoirement
    function birthOfBonus(brick) {
        if (Math.floor((Math.random() * 100) + 1) < 10) {
            if (isBonusFalling === false) {
                bonusType = Math.floor(Math.random() * 3) + 1;
                color = undefined;
                /**Bonus Types:
                 * 1 = Green Item - Add 10px to the bar
                 * 2 = Red Item - Remove 10px to the bar
                 * 3 = Add one ball
                 */
                if (bonusType === 1) {
                    color = "green";
                } else if (bonusType === 2) {
                    color = "red";
                } else if (bonusType === 3) {
                    color = "blue";
                }
                $("#gameArea").append("<div id='bonusBrick'></div>");
                $bonusBrick = $("#bonusBrick");
                $bonusBrick.css({
                    "position": "fixed",
                    "left": bricksList[brick].Left + ((bricksList[brick].Right - bricksList[brick].Left) / 2) - 25,
                    "top": bricksList[brick].Top + ((bricksList[brick].Bottom - bricksList[brick].Top) / 2) - 5,
                    "height": 10,
                    "width": 30,
                    "backgroundColor": color
                });
                topBonus = bricksList[brick].Top + ((bricksList[brick].Bottom - bricksList[brick].Top) / 2) - 5;
                fallingBonus($bonusBrick, bonusType);
                isBonusFalling = true;
            }
        }
    }

    // Fait tomber le bonus
    function fallingBonus(bonus, bonusType) {
        topBonus = parseInt(bonus.css("top"));
        leftBonus = parseInt(bonus.css("left"));
        widthBonus = parseInt(bonus.css("width"));
        heightBonus = parseInt(bonus.css("height"));
        fallbon = setInterval(function () {
            topBonus += 1;
            bonus.css("top", topBonus);
            if (topBonus + heightBonus >= $playingBarPosition.Top - 2 && $playingBarPosition.Top <= topBonus + heightBonus) {
                if ($playingBarPosition.Left <= leftBonus && $playingBarPosition.Right >= leftBonus + widthBonus) {
                    // console.log("playingBar.Left"+$playingBarPosition.Left + )
                    applyBonus(bonusType);
                    isBonusFalling = false;
                    bonus.remove();
                    clearInterval(fallbon);
                }
                if (topBonus >= $playingBarPosition.Bottom) {
                    isBonusFalling = false;
                    bonus.remove();
                    clearInterval(fallbon);
                }
            }
        }, 8);
    }

    // Applique le bonus s'il a été attrapé
    function applyBonus(bonusType) {
        if (bonusType === 1 && $playingBarPosition.Width < ($gameArea.width() / 2)) {
            $playingBar.animate({
                width: $playingBarPosition.Width + 10
            }, 200);
            $playingBar.css("backgroundColor", "rgb(" + randomColor()[0] + ", " + randomColor()[1] + ", " + randomColor()[2] + ")");
            $playingBarPosition.Width += 10;
        } else if (bonusType === 2 && $playingBarPosition.Width > (ballsList[0].Selector.width() / 2)) {
            $playingBar.animate({
                width: $playingBarPosition.Width - 10
            }, 200);
            $playingBar.css("backgroundColor", "rgb(" + randomColor()[0] + ", " + randomColor()[1] + ", " + randomColor()[2] + ")");
            $playingBarPosition.Width -= 10;
        } else if (bonusType === 3) {
            bonusBallAdder();
        }
    }

    // Ajoute une balle au plateau de jeu
    function bonusBallAdder() {
        try {
            clearInterval(bonusInt);
        } catch (e) {
        }
        bonusBallsHistory++;
        ballID = "bonusBall" + bonusBallsHistory;
        $gameArea.append("<div id='bonusBall'></div>");
        $("#bonusBall").attr("id", ballID);
        $bonusBall = $("#" + ballID);
        $bonusBall.css({
            "height": 10,
            "width": 10,
            "left": $playingBarPosition.Left + $playingBarPosition.Width,
            "Top": $playingBarPosition.Top,
            "backgroundColor": "red",
            "position": "fixed"
        });

        ballsList.push({
            "Selector": $bonusBall,
            "Top": $playingBarPosition.Top,
            "Left": $playingBarPosition.Left + $playingBarPosition.Width,
            "Angle": Math.floor(Math.random() * 170) + 10
        });

        bonusInt = setInterval(function () {
            for (var i = 1; i < ballsList.length; i++) {
                movingball(false, i);
            }
            if (ballsList.length === 1) {
                clearInterval(bonusInt);
            }
        }, 8);
    }

    //Fonction qui exécute le jeu FPSinMS = 8
    function play_game() {
        if (FPSinMS === 8) {
            playingInt = setInterval(function () {
                movingball(false, 0);
                if (FPSinMS === 4) {
                    callback();
                    clearInterval(playingInt);
                } else if (gameplaying === false) {
                    clearInterval(playingInt);
                }
            }, 8);
        }
    }

    //Fonction qui exécute le jeu FPSinMS = 4
    function callback() {
        callbackInt = setInterval(function () {
            movingball(false, 0);
            if (FPSinMS === 8) {
                play_game();
                clearInterval(callbackInt);
            } else if (gameplaying === false) {
                clearInterval(callbackInt);
            }
        }, 6);
    }

    function waitingForClick() {
        setInterval(function () {
            $(window).click(function () {
                if (gameplaying === false) {
                    FPSinMS = 8;
                    monitorMouseMove();
                    movingball(true, 0);
                    play_game();
                    gameplaying = true;
                }
            })
        }, 50);
    }

    generateBoard();
    $("#home").click(function () {
        //ramener vers page accueil
    });
    $("#refresh").click(function () {
        location.reload();
    });
    waitingForClick();

});