# Setting up IronTrak on GitHub

## 1. Initialize Git Repository

Open your terminal in the project folder (`c:\Users\SKTech\Desktop\project_files\projects\ONGOING\IronTrak`) and run:

```bash
git init
git add .
git commit -m "Initial commit: IronTrak Phase 2 Complete"
```

## 2. Create Repository on GitHub

1.  Go to [github.com/new](https://github.com/new).
2.  **Repository name:** `IronTrak`
3.  **Description:** Professional Digital Bandsaw Measurement System (Arduino Nano + 600PPR Encoder)
4.  **Public/Private:** Choose Public (Open Source) or Private.
5.  **Initialize this repository with:** Leave all unchecked (we have code already).
6.  Click **Create repository**.

## 3. Push Code to GitHub

Copy the commands GitHub shows you under "…or push an existing repository from the command line" and run them in your terminal. They will look like this:

```bash
git remote add origin https://github.com/YOUR_USERNAME/IronTrak.git
git branch -M main
git push -u origin main
```

*(Replace `YOUR_USERNAME` with your actual GitHub username)*

## 4. Verify

Refresh the GitHub page. You should see all your files, including the `docs/` folder and the nice `README.md` we created!

---

## Future Workflow

When you make changes:

1.  **Check status:** `git status`
2.  **Stage changes:** `git add .`
3.  **Commit:** `git commit -m "Description of changes"`
4.  **Push:** `git push`
