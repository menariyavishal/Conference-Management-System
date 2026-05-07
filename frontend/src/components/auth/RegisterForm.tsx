import React, { useState } from 'react';
import { useAuth } from '../../context/AuthContext';
import authService from '../../services/auth.service';
import { useNavigate } from 'react-router-dom';
import { toast } from 'react-hot-toast';
import type { UserRole } from '../../types/api';

interface RegisterFormProps {
  onSwitch: () => void;
}

const INTEREST_OPTIONS = [
  { id: 'AI', label: '🤖 AI' },
  { id: 'Quantum', label: '⚛️ Quantum' },
  { id: 'Cybersecurity', label: '🔒 Cybersecurity' },
  { id: 'BioTech', label: '🧬 BioTech' },
  { id: 'HCI', label: '🖥️ HCI' },
];

const RegisterForm: React.FC<RegisterFormProps> = ({ onSwitch }) => {
  const [formData, setFormData] = useState({
    firstName: '',
    lastName: '',
    email: '',
    password: '',
    confirmPassword: '',
    affiliation: '',
    role: 'AUTHOR' as UserRole,
  });
  const [selectedInterests, setSelectedInterests] = useState<string[]>([]);
  const [isLoading, setIsLoading] = useState(false);
  
  const { login } = useAuth();
  const navigate = useNavigate();

  const toggleInterest = (id: string) => {
    setSelectedInterests(prev => 
      prev.includes(id) ? prev.filter(i => i !== id) : [...prev, id]
    );
  };

  const handleSubmit = async (e: React.FormEvent) => {
    e.preventDefault();
    if (formData.password !== formData.confirmPassword) {
      toast.error("Passwords do not match");
      return;
    }
    
    setIsLoading(true);
    try {
      // Transform frontend data to backend format
      const registrationData = {
        email: formData.email,
        password: formData.password,
        fullName: `${formData.firstName} ${formData.lastName}`,
        role: formData.role.toLowerCase(),
        affiliation: formData.affiliation,
        interests: selectedInterests
      };
      
      console.log("Sending registration data:", registrationData);
      const response = await authService.register(registrationData);
      console.log("Registration response:", response);
      
      if (response.success) {
        toast.success("Account created successfully!");
        // Auto-login after registration
        try {
          const loginRes = await authService.login({ email: formData.email, password: formData.password });
          console.log("Login response after registration:", loginRes);
          if (loginRes.success) {
            const userData = {
              id: loginRes.data.userId || loginRes.data.id,
              userId: loginRes.data.userId,
              email: loginRes.data.email,
              fullName: loginRes.data.fullName,
              role: loginRes.data.role?.toUpperCase(),
              isActive: true,
              createdAt: new Date().toISOString(),
              updatedAt: new Date().toISOString()
            };
            login(loginRes.data.token, userData as any);
            navigate('/dashboard');
          }
        } catch (loginError) {
          console.error("Auto-login failed:", loginError);
          toast("Account created! Please log in manually.");
          navigate('/login');
        }
      } else {
        // Handle error response - could be error.message or error.error.message
        const errorMsg = typeof response.error === 'object' 
          ? response.error?.message 
          : response.error || "Registration failed";
        toast.error(String(errorMsg));
      }
    } catch (error: any) {
      console.error("Registration error:", error);
      toast.error(error.message || error || "Registration failed");
    } finally {
      setIsLoading(false);
    }
  };

  return (
    <div className="p-8">
      <div className="text-center mb-6">
        <h2 className="text-3xl font-bold bg-gradient-to-r from-blue-400 to-purple-400 bg-clip-text text-transparent">
          Create Account
        </h2>
        <p className="text-slate-400 mt-1">Join the research community</p>
      </div>

      <form onSubmit={handleSubmit} className="space-y-5">
        <div className="grid grid-cols-2 gap-4">
          <div>
            <label className="block text-xs font-bold text-slate-500 uppercase tracking-wider mb-2">First Name</label>
            <input
              type="text"
              required
              className="input-field"
              value={formData.firstName}
              onChange={(e) => setFormData({ ...formData, firstName: e.target.value })}
            />
          </div>
          <div>
            <label className="block text-xs font-bold text-slate-500 uppercase tracking-wider mb-2">Last Name</label>
            <input
              type="text"
              required
              className="input-field"
              value={formData.lastName}
              onChange={(e) => setFormData({ ...formData, lastName: e.target.value })}
            />
          </div>
        </div>

        <div>
          <label className="block text-xs font-bold text-slate-500 uppercase tracking-wider mb-2">Email Address</label>
          <input
            type="email"
            required
            className="input-field"
            value={formData.email}
            onChange={(e) => setFormData({ ...formData, email: e.target.value })}
          />
        </div>

        <div className="grid grid-cols-2 gap-4">
          <div>
            <label className="block text-xs font-bold text-slate-500 uppercase tracking-wider mb-2">Password</label>
            <input
              type="password"
              required
              minLength={8}
              className="input-field"
              value={formData.password}
              onChange={(e) => setFormData({ ...formData, password: e.target.value })}
            />
          </div>
          <div>
            <label className="block text-xs font-bold text-slate-500 uppercase tracking-wider mb-2">Confirm</label>
            <input
              type="password"
              required
              className="input-field"
              value={formData.confirmPassword}
              onChange={(e) => setFormData({ ...formData, confirmPassword: e.target.value })}
            />
          </div>
        </div>

        <div>
          <label className="block text-xs font-bold text-slate-500 uppercase tracking-wider mb-2">Affiliation</label>
          <input
            type="text"
            required
            placeholder="University or Company"
            className="input-field"
            value={formData.affiliation}
            onChange={(e) => setFormData({ ...formData, affiliation: e.target.value })}
          />
        </div>

        <div>
          <label className="block text-xs font-bold text-slate-500 uppercase tracking-wider mb-2">Primary Role</label>
          <div className="relative">
            <select
              className="input-field appearance-none cursor-pointer pr-10"
              value={formData.role}
              onChange={(e) => setFormData({ ...formData, role: e.target.value as UserRole })}
            >
              <option value="AUTHOR">Author (Submit papers)</option>
              <option value="REVIEWER">Reviewer (Evaluate papers)</option>
              <option value="ORGANIZER">Organizer (Manage events)</option>
            </select>
            <div className="absolute right-4 top-1/2 -translate-y-1/2 pointer-events-none text-slate-400">
              <svg className="w-4 h-4" fill="none" stroke="currentColor" viewBox="0 0 24 24"><path strokeLinecap="round" strokeLinejoin="round" strokeWidth="2" d="M19 9l-7 7-7-7"></path></svg>
            </div>
          </div>
        </div>

        <div>
          <label className="block text-sm font-semibold text-slate-700 mb-3">Research Interests</label>
          <div className="flex flex-wrap gap-2">
            {INTEREST_OPTIONS.map(opt => (
              <span
                key={opt.id}
                onClick={() => toggleInterest(opt.id)}
                className={`px-4 py-2 rounded-lg text-xs font-bold cursor-pointer transition-all border ${
                  selectedInterests.includes(opt.id)
                    ? 'bg-blue-600 border-blue-600 text-white'
                    : 'bg-white border-slate-200 text-slate-600 hover:border-blue-400'
                }`}
              >
                {opt.label}
              </span>
            ))}
          </div>
        </div>

        <button
          type="submit"
          disabled={isLoading}
          className="btn-primary w-full py-4 text-base font-bold mt-4"
        >
          {isLoading ? 'Registering...' : 'Create Account'}
        </button>
      </form>

      <div className="mt-8 text-center">
        <p className="text-slate-500 mb-6">
          Already have an account?{' '}
          <button
            type="button"
            onClick={onSwitch}
            className="text-blue-600 hover:underline font-bold"
          >
            Sign In
          </button>
        </p>
        
        <p className="text-slate-400 text-[10px] opacity-60 tracking-widest uppercase">
          🔐 SECURE ENCRYPTED CONNECTION
        </p>
      </div>
    </div>
  );
};

export default RegisterForm;
