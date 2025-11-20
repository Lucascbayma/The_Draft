<h1 align="center">🖊️ The Draft</h1>

<div align="center">
    <img src="images/titulo1.png" alt="Tela de início do The Draft" width="80%">
    <br><br>
</div>

<br>

## 👥 Equipe de Desenvolvimento

| Nome | Função | GitHub |
| :--- | :--- | :--- |
| **Eros Amancio** | Desenvolvedor | [@erosamancio](https://github.com/erosamancio) |
| **Lucas Bayma** | Desenvolvedor | [@lucascbayma](https://github.com/lucascbayma) |
| **Pedro Moreira** | Desenvolvedor | [@Pedromoreira18](https://github.com/PedroMoreira18) |

---

## 📖 Sobre o Jogo

**The Draft** é um *roguelike* de ação top-down, com forte inspiração no clássico *The Binding of Isaac: Rebirth*, mas que transporta o jogador para um mundo onde tudo é feito de **papel** e desenhado à mão.

Assuma o controle do **Rabisco**, um pequeno ser feito de rascunhos, que deve lutar por um mundo cheio de hordas de inimigos com seu **lápis** afiado! ✏️

### 🎯 Jogabilidade e Mecânicas

* **Personagem:** **Rabisco**, o protagonista desenhado.
* **Moeda:** Colete **clipes de papel** 📎 derrotando inimigos para gastar na loja.
* **Combate:** Enfrente **4 tipos de inimigos** com inteligência artificial distinta:
    * **Padrão:** Persegue o jogador.
    * **Tanque:** Mais vida, porém mais lento.
    * **Aranha:** Movimento errático e rápido, evita paredes.
    * **Atirador:** Ataca à distância com projéteis de borracha.

* **Loja e Progressão:** O jogo conta com pedestais que vendem itens aleatórios para fortalecer o Rabisco:
    * ❤️ **Corações:** Recuperam vida (Metade, Inteiro ou Duplo).
    * 🩹 **Grampeador:** Aumenta a vida máxima.
    * ⚡️ **Velocidade:** Aumenta a rapidez de movimento.
    * 💥 **Dano:** Aumenta o poder de ataque do lápis.
    * 📏 **Alcance:** Aumenta a distância do golpe.

---

## 🕹️ Controles

O jogo suporta Teclado e Controle.

| Ação | Teclado | Gamepad (PlayStation/Xbox) |
| :--- | :--- | :--- |
| **Mover** | `W`, `A`, `S`, `D` | Analógico Esquerdo / D-Pad |
| **Atacar** | `Setas` (↑, ↓, ←, →) | `Botões` (△, ◯, X, □ / Y, B, A, X) |
| **Interagir/Comprar** | `Espaço` ou `Enter` | Botão de Ação (X / A) |
| **Pausar/Sair** | `ESC` | Start |

---

## 🛠️ Tecnologias Utilizadas

* **Linguagem:** C
* **Biblioteca Gráfica:** [Raylib](https://www.raylib.com/)
* **Compilação:** GCC
* **Arte:** Procreate (iPad)

---

## 💻 Como Baixar e Compilar

Para jogar **The Draft**, você precisará clonar o repositório e compilar o código-fonte.

### 1. Pré-requisitos

Certifique-se de ter instalado em sua máquina:
* **Git**
* **Compilador C** (GCC)
* **Raylib** (Instalada via Homebrew no Mac ou gerenciador de pacotes no Linux)

### 2. Clonando o Repositório

Abra seu terminal e execute:

```bash
[git clone https://github.com/SEU_USUARIO/the_draft.git](https://github.com/Lucascbayma/The_Draft.git)
cd the_draft
```

### 3. Compilando e Rodando

#### 🍎 macOS (Recomendado - Via Homebrew)

Se você usa Mac e instalou a Raylib via `brew`, utilize este comando para garantir que todos os arquivos (`main`, `rabisco` e `inimigos`) sejam linkados corretamente:

```bash
clang main.c rabisco.c inimigos.c -o the_draft \
-I/opt/homebrew/include -L/opt/homebrew/lib \
-lraylib -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo
```

Para rodar o jogo:

```bash
./the_draft
```

#### 🐧 Linux (Ubuntu/Debian)

Certifique-se de ter as dependências instaladas e rode:

```bash
gcc main.c rabisco.c inimigos.c -o the_draft -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
```

Para rodar: 

```bash
./the_draft
```

#### 🪟 Windows

> ⚠️ **Nota:** O jogo foi desenvolvido e testado apenas em macOS e Linux. A compatibilidade com Windows não foi verificada, mas caso queira testar utilizando MinGW:

```bash
gcc main.c rabisco.c inimigos.c -o the_draft.exe -lraylib -lopengl32 -lgdi32 -lwinmm
```

---

<div align="center"> 
    <h3>🎬 Vídeo Demonstrativo</h3> 
    <a href="[https://www.youtube.com/watch?v=ID_DO_VIDEO](https://www.youtube.com/watch?v=ID_DO_VIDEO)"> 
        <img src="[https://img.youtube.com/vi/ID_DO_VIDEO/0.jpg](https://img.youtube.com/vi/ID_DO_VIDEO/0.jpg)" alt="The Draft Gameplay" width="600"> 
    </a> 
    <p><i>Clique na imagem para assistir um pouco da gameplay do The Draft</i></p> 
</div>
