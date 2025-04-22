// BlackJack Game in C by Jace from www.siteraw.com

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define INIT_CASH 500
#define MAX_CARDS 11 // Maximum cards a player can theoretically hold
#define DECK_SIZE 52 // Standard deck size

// Card representation
const char suits[] = {'\3', '\4', '\5', '\6'}; // Heart, Diamond, Club, Spade
const char *ranks[] = {"A", "2", "3", "4", "5", "6", "7", "8", "9", "0", "J", "Q", "K"}; // Use "T", "X" or "0" for 10

// Struct to store a card
typedef struct {
    int suit;
    int rank;
} Card;

// Function prototypes
void display_menu();
void show_rules();
void play_game(int *cash, int *wins, int *losses);
int deal_card(Card *cards, int card_count, int *used_cards);
int calculate_total(Card *cards, int card_count);
void display_cards(Card *cards, int card_count, const char *player, int show_mystery);
int get_bet(int cash);
void dealer_turn(Card *dealer_cards, int *dealer_count, int *dealer_total, int *used_cards);

int main() {
    srand((unsigned)time(NULL));
    int cash = INIT_CASH, wins = 0, losses = 0;
    char choice;

    while (1) {
        display_menu();
        scanf(" %c", &choice);
        if (choice == '1') {
            play_game(&cash, &wins, &losses);
        } else if (choice == '2') {
            show_rules();
        } else if (choice == '3') {
            printf("Thanks for playing!\n");
            break;
        }
    }
    return 0;
}

void display_menu() {
    system("cls");
    printf("=== === ===== = ===== === ===\n");
    printf("=== SiteRaw.com Blackjack ===\n");
    printf("=== == === ======= === == ===\n\n");
    printf("1. Play Game\n2. Show Rules\n3. Exit\n");
    printf("Choice: ");
}

void show_rules() {
    system("cls");
    printf("=== Rules of Blackjack ===\n");
    printf("- Cards are drawn randomly from a 52-card deck.\n");
    printf("- Number cards (2-10) are worth their face value.\n");
    printf("- J, Q, K are worth 10; A is worth 1 or 11.\n");
    printf("- Goal: Get as close to 21 as possible without going over.\n");
    printf("- Beat the dealer's hand to win.\n");
    printf("- Natural Blackjack (21 with first two cards) pays 3:2.\n");
    printf("- Hit to draw a card; Stay to stop.\n");
    printf("- Visit www.siteraw.com\n");
    printf("Press any key to return to menu...\n");
    scanf(" %*c");
}

void play_game(int *cash, int *wins, int *losses) {
    Card player_cards[MAX_CARDS], dealer_cards[MAX_CARDS];
    int player_count = 0, dealer_count = 0, player_total, dealer_total;
    char action;
    int used_cards[DECK_SIZE] = {0}; // Track used cards (0 = unused, 1 = used)

    while (*cash > 0) {
        system("cls");
        printf("Cash: $%d | Wins: %d | Losses: %d\n", *cash, *wins, *losses);

        // Reset for new round
        player_count = dealer_count = 0;
        for (int i = 0; i < DECK_SIZE; i++) used_cards[i] = 0; // Reset deck

        // Initial deal: two cards to player, two to dealer
        deal_card(player_cards, player_count++, used_cards);
        deal_card(player_cards, player_count++, used_cards);
        deal_card(dealer_cards, dealer_count++, used_cards);
        deal_card(dealer_cards, dealer_count++, used_cards);
        player_total = calculate_total(player_cards, player_count);
        dealer_total = calculate_total(dealer_cards, dealer_count);

        display_cards(dealer_cards, dealer_count, "Dealer", 1); // Show mystery card
        printf("Dealer Total: %d\n\n", calculate_total(dealer_cards, 1)); // Show only first card's value
        display_cards(player_cards, player_count, "Player", 1);
        printf("Player Total: %d\n\n", calculate_total(player_cards, 1));

        int bet = get_bet(*cash);
	display_cards(player_cards, player_count, "Player", 0);
        printf("Player Total: %d\n\n", player_total);

        // Check for natural blackjack (21 with exactly 2 cards)
        int natural_blackjack = (player_count == 2 && player_total == 21);
        if (natural_blackjack) {
            printf("Natural Blackjack! You win 3:2!\n");
            (*wins)++;
            *cash += (bet * 3) / 2;
            display_cards(dealer_cards, dealer_count, "Dealer", 0);
            printf("Dealer Total: %d\n", dealer_total);
            goto continue_round;
        }

        // Player's turn
        while (player_total < 21) {
            printf("Hit or Stay? (H/S): ");
            scanf(" %c", &action);
            if (action == 'H' || action == 'h') {
                deal_card(player_cards, player_count++, used_cards);
                player_total = calculate_total(player_cards, player_count);
                display_cards(dealer_cards, dealer_count, "Dealer", 1);
                printf("Dealer Total: %d\n\n", calculate_total(dealer_cards, 1));
                display_cards(player_cards, player_count, "Player", 0);
                printf("Player Total: %d\n", player_total);
            } else if (action == 'S' || action == 's') {
                printf("You stay at %d.\n", player_total);
                break;
            }
            if (player_total > 21) {
                printf("Bust! You lose.\n");
                (*losses)++;
                *cash -= bet;
                goto continue_round;
            } else if (player_total == 21) {
                printf("You win!\n");
                (*wins)++;
                *cash += bet;
                break;
            }
        }

        // Dealer's turn if player didn't bust
        if (player_total <= 21) {
            dealer_turn(dealer_cards, &dealer_count, &dealer_total, used_cards);
            display_cards(dealer_cards, dealer_count, "Dealer", 0);
            printf("Dealer Total: %d\n", dealer_total);

            if (dealer_total > 21 || player_total > dealer_total) {
                printf("You win!\n");
                (*wins)++;
                *cash += bet;
            } else if (player_total < dealer_total) {
                printf("Dealer wins.\n");
                (*losses)++;
                *cash -= bet;
            } else {
                printf("Push! It's a tie.\n");
            }
        }

    continue_round:
        if (*cash <= 0) {
            printf("Bankrupt! Game Over.\n");
            *cash = INIT_CASH;
        }

        printf("Play again? (Y/N): ");
        scanf(" %c", &action);
        if (action == 'N' || action == 'n') {
            break;
        }
    }
}

int deal_card(Card *cards, int card_count, int *used_cards) {
    Card card;
    int card_index;
    do {
	card_index = rand() % 52;
        card.suit = card_index / 13;
        card.rank = card_index % 13;
        //card_index = card.suit * 13 + card.rank; // Unique index for each card
    } while (used_cards[card_index]); // Keep trying until unused card is found
    used_cards[card_index] = 1; // Mark card as used
    cards[card_count] = card;
    return card.rank;
}

int calculate_total(Card *cards, int card_count) {
    int total = 0, aces = 0;
    
    for (int i = 0; i < card_count; i++) {
        int value = cards[i].rank + 1;
        if (value > 10) value = 10; // J, Q, K
        if (value == 1) {
            aces++;
            value = 11; // Initially count Ace as 11
        }
        total += value;
    }
    
    // Adjust for aces if total > 21
    while (total > 21 && aces > 0) {
        total -= 10; // Change one Ace from 11 to 1
        aces--;
    }
    
    return total;
}

void display_cards(Card *cards, int card_count, const char *player, int show_mystery) {
    printf("%s's Cards:\n", player);
    for (int i = 0; i < (show_mystery ? card_count-1 : card_count); i++) {
        printf("------- ");
    }
    if (show_mystery && card_count > 1) printf("------- ");
    printf("\n");
    for (int i = 0; i < card_count; i++) {
        if (show_mystery && i > 0) continue; // Skip showing dealer's other cards
        printf("|%c    | ", suits[cards[i].suit]);
    }
    if (show_mystery && card_count > 1) printf("|     | ");
    printf("\n");
    for (int i = 0; i < card_count; i++) {
        if (show_mystery && i > 0) continue;
        printf("|  %-2s | ", ranks[cards[i].rank]);
    }
    if (show_mystery && card_count > 1) printf("|     | ");
    printf("\n");
    for (int i = 0; i < card_count; i++) {
        if (show_mystery && i > 0) continue;
        printf("|    %c| ", suits[cards[i].suit]);
    }
    if (show_mystery && card_count > 1) printf("|     | ");
    printf("\n");
    for (int i = 0; i < (show_mystery ? card_count-1 : card_count); i++) {
        printf("------- ");
    }
    if (show_mystery && card_count > 1) printf("------- ");
    printf("\n");
}

int get_bet(int cash) {
    int bet;
    printf("Enter Bet: $");
    scanf("%d", &bet);
    while (bet > cash || bet <= 0) {
        printf("Invalid bet. Enter Bet (1-%d): $", cash);
        scanf("%d", &bet);
    }
    return bet;
}

void dealer_turn(Card *dealer_cards, int *dealer_count, int *dealer_total, int *used_cards) {
    while (*dealer_total < 17) {
        deal_card(dealer_cards, (*dealer_count)++, used_cards);
        *dealer_total = calculate_total(dealer_cards, *dealer_count);
    }
}
