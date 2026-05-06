<div align="center">

# 🎨 Conference Management System — Frontend

<p align="center">
  A premium, full-featured React frontend for the Conference Management System. Built with React 18, TypeScript, and Vite, featuring a glassmorphism design language, Framer Motion animations, and a fully typed API service layer that communicates with the C++17 backend.
</p>

<p align="center">
  <img src="https://img.shields.io/badge/React-18-20232A?style=for-the-badge&logo=react&logoColor=61DAFB" alt="React 18"/>
  <img src="https://img.shields.io/badge/TypeScript-5-3178C6?style=for-the-badge&logo=typescript&logoColor=white" alt="TypeScript 5"/>
  <img src="https://img.shields.io/badge/Vite-6-646CFF?style=for-the-badge&logo=vite&logoColor=white" alt="Vite 6"/>
  <img src="https://img.shields.io/badge/Tailwind_CSS-4-38B2AC?style=for-the-badge&logo=tailwind-css&logoColor=white" alt="Tailwind CSS"/>
  <img src="https://img.shields.io/badge/Framer_Motion-12-0055FF?style=for-the-badge&logo=framer&logoColor=white" alt="Framer Motion"/>
  <img src="https://img.shields.io/badge/Axios-1.x-5A29E4?style=for-the-badge&logo=axios&logoColor=white" alt="Axios"/>
</p>

</div>

---

## 📑 Table of Contents

1. [📁 Project Structure](#-project-structure)
2. [🚀 Getting Started](#-getting-started)
3. [🎨 Design System](#-design-system)
4. [🏗️ Architecture & Key Decisions](#️-architecture--key-decisions)
5. [📦 Implemented Components](#-implemented-components)
6. [🌐 API Service Layer](#-api-service-layer)
7. [🔐 Authentication Flow](#-authentication-flow)
8. [📋 TypeScript Configuration](#-typescript-configuration)
9. [🛠️ Development Notes & Known Issues](#️-development-notes--known-issues)

---

## 📁 Project Structure

```
frontend/
├── public/                     # Static assets
├── src/
│   ├── assets/                 # Images, icons, fonts
│   │
│   ├── components/             # Reusable UI components
│   │   ├── animations/         # Shared animation wrapper components
│   │   ├── auth/
│   │   │   ├── LoginForm.tsx           # Email/password login form
│   │   │   ├── RegisterForm.tsx        # Role-aware registration form
│   │   │   └── ProtectedRoute.tsx      # Route guard HOC
│   │   ├── dashboard/
│   │   │   ├── AuthorDashboard.tsx     # Author-specific dashboard view
│   │   │   ├── OrganizerDashboard.tsx  # Organizer-specific dashboard view
│   │   │   ├── ReviewerDashboard.tsx   # Reviewer-specific dashboard view
│   │   │   └── NewSubmissionModal.tsx  # Manuscript submission modal
│   │   └── layout/
│   │       └── DashboardLayout.tsx     # Persistent nav shell for all dashboards
│   │
│   ├── context/
│   │   └── AuthContext.tsx     # Global authentication state provider
│   │
│   ├── hooks/                  # Custom React hooks
│   │
│   ├── pages/
│   │   ├── AuthPage.tsx        # Unified login/register page
│   │   ├── AuthPage.css        # Page-specific background styles
│   │   └── DashboardPage.tsx   # Page that renders the correct dashboard by role
│   │
│   ├── services/               # Axios-based API service layer
│   │   ├── api.ts              # Base Axios instance with interceptors
│   │   ├── auth.service.ts     # Login & registration API calls
│   │   ├── submission.service.ts  # Submission CRUD + file upload
│   │   ├── review.service.ts   # Review assignment & scoring API calls
│   │   ├── session.service.ts  # Conference session API calls
│   │   └── reporting.service.ts   # Analytics & reporting API calls
│   │
│   ├── types/
│   │   └── api.ts              # Shared TypeScript interfaces (User, Submission, Review...)
│   │
│   ├── utils/                  # Utility functions (date formatting, etc.)
│   │
│   ├── App.tsx                 # Root component with React Router configuration
│   ├── App.css                 # Global animation keyframes
│   ├── index.css               # Tailwind base directives & global styles
│   └── main.tsx                # Application entry point
│
├── index.html                  # Root HTML template
├── vite.config.ts              # Vite configuration (proxy, plugins)
├── tsconfig.app.json           # TypeScript config for the app source
├── tsconfig.node.json          # TypeScript config for build tooling (vite.config.ts)
└── eslint.config.js            # ESLint configuration
```

---

## 🚀 Getting Started

### Prerequisites
- **Node.js 18+** and **npm** installed.
- The **backend API** must be running on `http://localhost:8080` (see the root `README.md`).

### Installation & Running

```bash
# Navigate to the frontend directory
cd frontend

# Install all dependencies
npm install

# Start the development server with Hot Module Replacement (HMR)
npm run dev
```

The app will be served at **`http://localhost:5173`**. The Vite dev server is configured to proxy all `/api` requests to `http://localhost:8080`, eliminating CORS issues during development.

### Building for Production

```bash
npm run build
```

The optimized static output will be placed in `frontend/dist/`. This can be served by any static file server or the backend itself.

---

## 🎨 Design System

The entire application is built on a unified dark-mode, glassmorphism design language. The core tokens are:

### Color Palette

| Token | Value | Usage |
|---|---|---|
| Background (Deep) | `#020617` (slate-950) | Page background |
| Background (Surface) | `#0f172a` (slate-900) | Cards, modals |
| Border | `rgba(255,255,255,0.1)` | Card borders |
| Accent Primary | `#3b82f6` (blue-500) | CTAs, links, focus rings |
| Accent Success | `#10b981` (emerald-500) | Accepted status badges |
| Accent Warning | `#f59e0b` (amber-500) | Pending status badges |
| Accent Danger | `#ef4444` (red-500) | Rejected status, errors |
| Text Primary | `#ffffff` | Headings, primary text |
| Text Secondary | `#94a3b8` (slate-400) | Body, captions |
| Text Muted | `#475569` (slate-600) | Placeholders, labels |

### Typography
- **Font Family**: `Inter` (loaded via Google Fonts), with fallback to system sans-serif.
- **Weights used**: 500 (medium), 700 (bold), 900 (black / ultra-bold headings).
- **Label style**: `text-xs font-black uppercase tracking-widest` for all form labels.

### Glassmorphism Effect
All cards, modals, and the navigation bar use this pattern:
```css
background: rgba(255, 255, 255, 0.03); /* Very subtle white tint */
border: 1px solid rgba(255, 255, 255, 0.10); /* Faint border */
backdrop-filter: blur(24px); /* The key: blurs everything behind the element */
```

### Animations
All animations are powered by **Framer Motion 12**. The variants used throughout the app are typed with `import type { Variants } from 'framer-motion'` to comply with the `verbatimModuleSyntax` TypeScript rule.

**Standard card entrance animation:**
```ts
const containerVariants: Variants = {
  hidden: { opacity: 0 },
  show: { opacity: 1, transition: { staggerChildren: 0.1 } }
};

const itemVariants: Variants = {
  hidden: { opacity: 0, y: 30 },
  show: { opacity: 1, y: 0, transition: { type: 'spring', stiffness: 260, damping: 20 } }
};
```

**Modal entrance animation:**
```ts
initial={{ opacity: 0, scale: 0.9, y: 40 }}
animate={{ opacity: 1, scale: 1, y: 0 }}
exit={{ opacity: 0, scale: 0.9, y: 40 }}
```

---

## 🏗️ Architecture & Key Decisions

### 1. API Proxy via Vite
Rather than configuring CORS on both servers during development, Vite's `server.proxy` in `vite.config.ts` transparently forwards all requests starting with `/api` from the frontend dev server (`:5173`) to the backend (`:8080`). This is transparent to the application code.

### 2. Auth State in React Context
Authentication state (`user` object, `token` string) is stored in a `React.createContext` provider (`AuthContext.tsx`). On mount, the context hydrates from `localStorage` so the user stays logged in across page refreshes. The `logout` function clears both in-memory state and `localStorage`.

### 3. Role-Based Dashboard Rendering
The `DashboardPage.tsx` reads the `user.role` from the `AuthContext` and conditionally renders the correct dashboard component (`AuthorDashboard`, `ReviewerDashboard`, or `OrganizerDashboard`). This avoids the need for multiple protected routes per role.

### 4. Axios Interceptors for JWT
The base `api.ts` Axios instance has a **request interceptor** that reads the token from `localStorage` on every request and appends it as:
```
Authorization: Bearer <token>
```
This means no component or service needs to manually handle auth headers.

### 5. FormData for File Uploads
The `submission.service.ts` `create()` method detects whether the input is a `FormData` object and, if so, sets `Content-Type: multipart/form-data` on the request. This allows the same service method to handle both plain JSON (for simple updates) and file upload submissions.

### 6. Button-to-Form Linking
The New Submission Modal has a scrollable form area and a **sticky footer** with the submit button. Because the button is outside the `<form>` element in the DOM, it uses the `form="submission-form"` HTML attribute linked to `id="submission-form"` on the `<form>` element. This is a standard HTML5 technique to trigger form submission from an external button.

---

## 📦 Implemented Components

### `components/auth/LoginForm.tsx`
- Email and password input fields with icon decorations.
- Client-side required-field validation before making the API call.
- Calls `auth.service.ts → login()` and saves the returned `{ user, token }` to `AuthContext`.
- Uses `react-hot-toast` for success and error notifications.
- Animated entrance with Framer Motion.

### `components/auth/RegisterForm.tsx`
- Fields: Full Name, Email, Password, Confirm Password.
- **Role Selector**: Three styled toggle cards (Author, Reviewer, Organizer) for role selection.
- Password confirmation validation on the client side.
- Calls `auth.service.ts → register()` and auto-logs the user in on success.
- Detailed toast notifications for both success and server-side validation errors.

### `components/auth/ProtectedRoute.tsx`
- A Higher-Order Component (HOC) that wraps any React Router `<Route>`.
- Reads `AuthContext`: if no user is present, immediately redirects to `/auth`.
- Optionally accepts a `requiredRole` prop to restrict access to specific roles.

### `components/layout/DashboardLayout.tsx`
- A persistent shell layout rendered by React Router as a parent route, using `<Outlet />` to render child dashboard pages.
- **Top Navigation Bar**: Sticky, glassmorphism-styled bar with the app logo, page title, a notification bell icon, and a user profile section (avatar placeholder, display name, role badge).
- **Logout Button**: Calls `AuthContext → logout()` and redirects to `/auth`.
- **Mobile Responsive**: Collapses navigation on smaller screens.

### `components/dashboard/AuthorDashboard.tsx`
- Fetches the author's submissions on mount via `submissionService.list()`.
- **Stats Section**: Four animated stat cards — Total, Pending, Accepted, Rejected — derived from the fetched submissions.
- **Submissions Table**: A styled list of all submitted papers with:
  - Paper title and truncated abstract.
  - Color-coded status badge (emerald for Accepted, amber for Pending, red for Rejected, blue for Submitted).
  - Submission date.
  - A "View Details" chevron action.
- **New Submission button**: Opens `NewSubmissionModal`.
- Re-fetches data after a successful submission via the `onSuccess` callback.

### `components/dashboard/OrganizerDashboard.tsx`
- Overview of active conferences, total submissions, sessions, and registered participants.
- Quick-action cards for managing conferences and assigning reviewers.

### `components/dashboard/ReviewerDashboard.tsx`
- Lists all review assignments for the logged-in reviewer.
- Each row shows the paper title, assigned date, and review deadline.
- Quick-action to navigate to the full review form.

### `components/dashboard/NewSubmissionModal.tsx`
- Full-screen overlay with animated modal container.
- **Form Fields** (all required):
  - **Title**: Paper title (minimum 3 characters).
  - **Co-Authors**: Names of collaborating authors.
  - **Keywords**: Comma-separated keywords (required by backend validation).
  - **Abstract**: Full paper abstract (minimum 10 characters).
- **File Upload Area**: Clickable zone that opens a hidden `<input type="file">`. Validates:
  - **Type**: `application/pdf`, `application/msword`, `application/vnd.openxmlformats-officedocument.wordprocessingml.document`.
  - **Size**: Maximum 10 MB.
  - Displays the selected filename and size on success.
- **Sticky Footer**: Contains Discard and Submit buttons. Submit is linked to the scrollable form via `form="submission-form"`.
- On submission, packages all data into `FormData` and calls `submissionService.create()`.

---

## 🌐 API Service Layer

### `services/api.ts`
The core Axios instance. Configured with:
- `baseURL` pointing to `/api/v1` (proxied to the backend via Vite).
- A **request interceptor** that reads the JWT from `localStorage` and injects it into every request header.
- A **response interceptor** that transparently unwraps the Axios response envelope, so services receive the API data directly (not nested inside `response.data`).

### `services/auth.service.ts`
```ts
login(email: string, password: string): Promise<{ user: User; token: string }>
register(data: RegisterData): Promise<{ user: User; token: string }>
```

### `services/submission.service.ts`
```ts
create(data: FormData | SubmissionData): Promise<ApiResponse<Submission>>
list(): Promise<ApiResponse<Submission[]>>
get(id: string): Promise<ApiResponse<Submission>>
update(id: string, data: Partial<Submission>): Promise<ApiResponse<Submission>>
```
> `create()` automatically sets `Content-Type: multipart/form-data` when passed a `FormData` object.

### `services/review.service.ts`
```ts
getAssigned(reviewerId: string): Promise<ApiResponse<Review[]>>
submitReview(submissionId: string, data: ReviewData): Promise<ApiResponse<Review>>
```

### `services/session.service.ts`
```ts
getSessions(conferenceId: string): Promise<ApiResponse<Session[]>>
```

### `services/reporting.service.ts`
```ts
getStats(): Promise<ApiResponse<ConferenceStats>>
```

---

## 🔐 Authentication Flow

```
1. User visits the app
       ↓
2. App reads localStorage for token & user
       ↓
   ┌── Token found? ──────────────────────────────────────────────┐
   │   YES → Hydrate AuthContext, navigate to /dashboard           │
   │   NO  → ProtectedRoute redirects to /auth                    │
   └──────────────────────────────────────────────────────────────┘
       ↓
3. User submits Login form
       ↓
4. auth.service.ts → POST /api/v1/auth/login
       ↓
5. Backend validates credentials, issues JWT
       ↓
6. AuthContext stores { user, token } → localStorage
       ↓
7. App navigates to /dashboard
       ↓
8. DashboardPage reads user.role → renders correct dashboard
```

---

## 📋 TypeScript Configuration

The project uses two `tsconfig` files:
- **`tsconfig.app.json`**: For the `src/` application code. Uses `verbatimModuleSyntax: true`, which mandates that any import that is only a type must use the `import type` syntax.
- **`tsconfig.node.json`**: For the Vite configuration files (`vite.config.ts`), uses a separate, more permissive config.

**Key enforced rules:**
- `import type { Variants } from 'framer-motion'` — required for all Framer Motion types.
- `import type { Submission } from '../../types/api'` — required for all shared types.
- Failure to use `import type` for type-only imports results in a TypeScript compile error.

---

## 🛠️ Development Notes & Known Issues

### Hot Module Replacement (HMR)
Vite's HMR works out of the box. Saving any `.tsx`, `.ts`, or `.css` file will instantly reflect changes in the browser without a full reload.

### Rebuilding the Backend
If you change backend code and need to rebuild:
1. **Stop the `conference_api.exe` process** (Ctrl+C in its terminal) before running `cmake --build`.
2. The linker cannot overwrite a running `.exe` on Windows and will fail with `Permission Denied`.

### Token Issues After Server Restart
If you restart the backend with a different `JWT_SECRET`, all existing tokens will be invalid. You will see a `Token validation error: parse_error.101` in the server logs.
**Fix:** Clear the browser's `localStorage` and log in again.

### Submission Validation Limits (Backend)
The backend enforces these minimum lengths:
- **Title**: Minimum 3 characters, maximum 500 characters.
- **Abstract**: Minimum 10 characters, maximum 5000 characters.
- **Keywords**: Required, maximum 500 characters.

Submissions that don't meet these rules will be rejected with a `400 Bad Request` and a descriptive error message visible in the server console.
